/* SPDX-License-Identifier: GPL-3.0-only
 * Adapted from MIDI clock contributions by ChanceTheMaker and keremimo (2026).
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* Audio ISR state. Input timestamps keep USB/TRS jitter outside the render loop. */
static struct {
    uint32_t pos, rendered, last_ms, start_ms, rem, interval_ms;
    uint32_t pulse_samples, interp_q8, tempo_ms;
    uint8_t mode, have_pulse, tempo_valid, tempo_n;
} midi_clock;

static __attribute__((noinline)) void midi_clock_transport(uint32_t status, uint32_t ms)
{
    if (status == 0xFAu) {                         /* Start: step zero */
        midi_clock.pos = midi_clock.rendered = midi_clock.rem = 0;
        midi_clock.have_pulse = midi_clock.tempo_valid = 0;
        midi_clock.interval_ms = 0;
        midi_clock.start_ms = ms;
        seq_start();
    } else if (status == 0xFBu) {                  /* Continue: preserve the step */
        midi_clock.pos = midi_clock.rendered;
        midi_clock.rem = 0;
        midi_clock.have_pulse = midi_clock.tempo_valid = 0;
        midi_clock.interval_ms = 0;
        midi_clock.start_ms = ms;
        if (!song.playing)
            motion_begin();
        song.playing = 1;
    } else if (status == 0xFCu) {
        seq_stop();
    }
}

#if SKIFF_SPP
/* Song Position Pointer (F2 lsb msb, following MIDI clock): spp MIDI beats (1/16s) from the top. Each track goes to
 * the step that many 1/16s reach at its own STEP DIV and LEN, in its direction (FWD, REV, PING; RAND anywhere), so that
 * step fires on the next block; Continue (FB) then plays on from there, as the MIDI spec has it. The pattern's steps
 * only: a song's row is not moved. While playing it takes effect at once (a master scrubbing). */
static __attribute__((noinline)) void midi_clock_locate(uint32_t spp)
{
    uint32_t i, at = spp * (beat_samples() / 4u);    /* samples from the top: a MIDI beat is a 1/16 */
    for (i = 0; i < NTRK; i++) {
        track_t *t = &trk[i];
        uint32_t len = t->p[P_SLEN] > 0 ? (uint32_t)t->p[P_SLEN] : 1u, d = (uint32_t)t->p[P_DIR];
        uint32_t step = div_samples((uint32_t)t->p[P_SDIV]), n = step ? at / step : 0u, m, idx;
        t->nud = 0;                                  /* as seq_start: no NUDGE, skips, retrigs or ratchets pending */
        t->rskip_n = 0;
        t->rh_n = 0;
        t->rat_left = 0;
        t->seq_pos = 0x7FFFFFFF;                     /* the step after seq_idx fires on the next block */
        if (!n) {                                    /* the top: exactly as seq_start */
            t->seq_idx = (uint16_t)(len - 1u);
            t->seq_cnt = 0;
            continue;
        }
        m = (n - 1u) % len;                          /* the step before step n, the one "playing" now */
        switch (d) {
        case DIR_REV:
            idx = len - 1u - m;
            t->seq_down = 0;
            break;
        case DIR_PING: {                             /* 1 2 .. LEN .. 2 1 2 ..: a period of 2 LEN - 2 */
            uint32_t p = len > 1u ? 2u * len - 2u : 1u, k = (n - 1u) % p;
            idx = k < len ? k : p - k;
            t->seq_down = (uint8_t)(k >= len);
            break;
        }
        case DIR_RAND:
            idx = rng() % len;
            break;
        default:
            idx = m;
        }
        t->seq_idx = (uint16_t)idx;
        t->seq_cnt = (uint8_t)(m + 1u);              /* steps of this pass played (seq_pass_last, seq_at) */
    }
    launch_steps = spp;                              /* the launch points' 1/16s, as from the top (launch.c) */
    beat_n = spp / 4u % 4u;                          /* the beat of the bar (the ARP LED) */
    beat_pos = 0;
}
#endif

static __attribute__((noinline)) void midi_clock_pulse(uint32_t ms)
{
    uint32_t q;
    if (!midi_clock.tempo_valid) {
        midi_clock.tempo_valid = 1;
        midi_clock.tempo_ms = ms;
        midi_clock.tempo_n = 0;
    } else if (++midi_clock.tempo_n == 6u) {
        uint32_t dt = ms - midi_clock.tempo_ms;
        midi_clock.tempo_ms = ms;
        midi_clock.tempo_n = 0;
        /* Six clocks are a quarter of a beat. Reject gaps and corrupt bursts. */
        if (dt >= 62u && dt <= 375u) {
            uint32_t old_beat = beat_samples(), new_beat = (uint32_t)FS * dt / 250u;
            if (song.playing && new_beat != old_beat) {
                uint32_t i, ratio = (new_beat << 12) / old_beat;
                /* Preserve each track's fractional step phase when the master
                 * changes tempo. The next pulse then lands on its new boundary. */
                for (i = 0; i < NTRK; i++) {
                    if (trk[i].seq_pos < (uint32_t)FS * 2u)
                        trk[i].seq_pos = (uint32_t)(((uint64_t)trk[i].seq_pos * ratio + 2048u) >> 12);
                    if (trk[i].seq_off)
                        trk[i].seq_off = (uint32_t)(((uint64_t)trk[i].seq_off * ratio + 2048u) >> 12);
                }
                if (clk_pos < (uint32_t)FS * 2u)   /* (the metronome's beat as the steps) */
                    clk_pos = (uint32_t)(((uint64_t)clk_pos * ratio + 2048u) >> 12);
                if (launch_pos < (uint32_t)FS * 2u)   /* (Skiff: the launch points' too, launch.c) */
                    launch_pos = (uint32_t)(((uint64_t)launch_pos * ratio + 2048u) >> 12);
            }
            midi_beat_samples = new_beat;
            song.g[G_BPM] = (int16_t)clamp((int32_t)((15000u + dt / 2u) / dt), 40, 240);
        }
    }
    if (song.playing) {
        if (midi_clock.have_pulse) {
            uint32_t interval = ms - midi_clock.last_ms;
            if (interval >= 8u && interval <= 80u)
                midi_clock.interval_ms = interval;
            q = beat_samples() + midi_clock.rem;
            midi_clock.pos += q / 24u;
            midi_clock.rem = q % 24u;
        }
        midi_clock.have_pulse = 1;
    }
    midi_clock.pulse_samples = beat_samples() / 24u;
    if (!midi_clock.interval_ms)
        midi_clock.interval_ms = beat_samples() * 1000u / ((uint32_t)FS * 24u);
    midi_clock.interp_q8 = midi_clock.pulse_samples * 256u / midi_clock.interval_ms;
    midi_clock.last_ms = ms;
}

static uint32_t midi_clock_advance(uint32_t now)
{
    uint32_t target, elapsed, offset, n;
    if (!midi_clock.have_pulse)
        return 0;
    elapsed = now - midi_clock.last_ms;
    /* Interpolate to the next pulse, never across it before it arrives. */
    if (elapsed > midi_clock.interval_ms)
        elapsed = midi_clock.interval_ms;
    offset = elapsed * midi_clock.interp_q8 >> 8;
    if (offset >= midi_clock.pulse_samples)
        offset = midi_clock.pulse_samples - 1u;
    target = midi_clock.pos + offset;
    n = (int32_t)(target - midi_clock.rendered) > 0 ? target - midi_clock.rendered : 0u;
    if (n > (uint32_t)FS / 8u)
        n = (uint32_t)FS / 8u;
    midi_clock.rendered += n;
    return n;
}

