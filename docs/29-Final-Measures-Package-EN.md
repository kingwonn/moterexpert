# Staged Final Measures Package: Risk Register × Party Responsibilities × Near/Mid/Long Term (Singapore Meeting Edition)

> English edition of Document 28. Structure: §1 Risk register (the "why" behind every measure) → §2 Four-party × three-horizon measures matrix (the "who and when") → §3 IPM Gen-1.5 / Gen-2.0 requirement specifications (reverse-derived from failure physics) → §4 Meeting evidence exhibits → §5 Information gaps (internal annex). Parties: **CND Motors** (motor), **BPS** (control board + firmware, incl. LKS FOC library), **Luxshare R&D** (system integration, board hardware, component sourcing), **Luxshare Manufacturing** (EOL testing, cut-in management, data). Horizons: **Near term = now–end of August (containment & cut-in); Mid term = Sep–Nov (design improvements within the current platform); Long term = 3–12 months (platform-level iteration)**. Note: Singapore is the highest-failure-rate market worldwide (0.297%) — a Singapore-specific data page is mandatory.

## 1. Risk Register (graded; every measure below traces back to one of these)

| # | Risk | Mechanism in one sentence | Evidence grade | Field constraint | Owning party | Verification status |
|---|---|---|---|---|---|---|
| R1 | **In-slot winding-to-core insulation weak point, electrically activated at high line** (primary root cause) | Winding-process stress / insulation voids at slot edges → ≥330 V (Paschen minimum) ignites partial discharge → carbon tracking → core bridges the other phase in the same slot → one tooth-coil consumed (pop mode) or wire fuses open (burning-smell mode) | Six independent evidence lines (17:1 voltage ratio, 9/18 ground-insulation collapse, 3–30 h PD life, humidity gradient, multi-device board damage, slot geometry) | Dominant: 17/18 returned motors show winding damage | CND | Teardown localization (V1) **overdue** |
| R2 | First-coil du/dt concentration (refinement candidate of R1) | PWM voltage steps distribute unevenly across series coils; the line-end coil takes a disproportionate share | Inference (consistent with the single-coil signature in 6/7 shorted units) | To be discriminated | CND + Luxshare R&D | Add "which coil died — line-end or neutral-end?" to V1 teardown |
| R3 | Lead-termination weld process cluster (Apr-1 build) | Weld joint fracture → open circuit + intact ground insulation + normal winding | A (date-clustered, 4 units) | Independent small cluster | CND | Audit report pending |
| R4 | Board-side sustained overcurrent (locked vector / gate-driver latch / MCU hang) | An active vector held → DC bus driven through windings → die thermal death + magnet demagnetization | A (supplier experiment reproduced the end-game); no field evidence on trigger rate | Capped at ≤22% of returns (intact-motor share; likely lower) | BPS | S6 re-run with production firmware & protections: **must complete before the meeting** |
| R5 | Hot IPM failure-violence amplifier (the fuse, not the initiator) | High junction temperature → short-circuit withstand collapses / parasitic latch → die fails short → rupture | A (chamber: cold dies fail silently, hot dies pop) | 92% of pops in CN (high-line) | BPS + Luxshare R&D | Derating parameters to be documented |
| R6 | Capacitor energy dump + mains follow-through (the executioner) | Bus bank discharges 11–13 J at high line (1.9–2.7 J at low line) through the failed die, plus rectifier follow-through | A (forensics + experiment) | Violence = f(Udc) | Luxshare R&D | Input fuse review (M15) pending |
| R7 | Loss-of-sync / observer freeze (residual P-E path) | Estimated speed goes wrong → all speed-based protections blinded | C | Pending margin test | BPS (LKS library) | Plausibility check lands in the Aug-18 firmware |
| R8 | Grid anomalies (surge / broken neutral / overvoltage) | Excluded as primary cause (no MOV damage / multi-component carbonization signatures) | B | Isolated cases | Luxshare R&D | MOV verification |
| R9 | Protection arrives late at 40–50 °C ambient | All calibration was done at 25 °C; at hot ambient the same thresholds trip ~25 K late | A (self-evident from calibration records) | Overlaps the hot-climate failure region | BPS + Manufacturing | Aug-18 firmware + hot-chamber verification |
| R10 | Demagnetization ratchet (post-event degraded operation) | After a high-current event ψf drops → same power demands more current → degradation spiral | A (experiment: −44~49% surface flux) | Latent in the installed base | BPS | Iteration item |

## 2. Four-Party × Three-Horizon Measures Matrix

### CND Motors (the motor — owns the fire seed)

| Horizon | Measures | Acceptance / deliverable |
|---|---|---|
| **Near** | ① 100% strengthened EOL: 1200 V winding-to-core hi-pot + inter-turn surge test, cut-in batch code recorded; ② Slot insulation / overmold CAPA + winding tension & nozzle-wear control into SOP; ③ Termination process fix (Apr-1 cluster); ④ Provide 5 defect-seeded motors (Aug 17; ≥2 of them into the hot-chamber leg); ⑤ Support V1 teardown burn-point localization + first-coil discrimination; ⑥ Written commitment to Grade-2 pinhole limits on magnet wire + incoming-inspection data; ⑦ Complete the demagnetization experiment (pre/post same-fixture flux scans, ke, L-vs-angle, magnet temperature) | Hi-pot ladder distribution before/after; V1 localization report; zero burnouts in cut-in batches |
| **Mid** | ① 88-turn winding prototype + PQ re-test (with wire gauge locked at 0.16 mm this is the only efficiency route: several watts less loss at high line); ② Per-unit factory archive: R and ke recorded for every motor (baseline for firmware self-test and returns attribution); ③ Spare-rotor staircase demagnetization calibration (current × temperature → loss% map, feeding firmware demag thresholds) | Prototype PQ comparison report; archive data interface with shipments |
| **Long** | ① Insulation system upgrade: corona-resistant magnet wire / improved impregnation — a fundamental hardening against PWM stress (moves the design from "survives Paschen ignition" to "does not ignite"); ② Next-generation motor platform with larger electromagnetic margin (magnetic-circuit optimization or Φ29 derivative) | Accelerated-life comparison on a PWM-stress rig; platform charter |

### BPS (control board + firmware, incl. LKS library — owns interruption & evidence capture)

| Horizon | Measures | Acceptance / deliverable |
|---|---|---|
| **Near (Aug-18 build)** | ① V2.2 plus four additions: power-on self-test (3-phase resistance symmetry — blocks restart of damaged motors) / fault persistence (EEPROM counters surviving re-plug + fault snapshots; minimal black box) / ambient-temperature compensation / observer plausibility check; ② IPM over-temperature derating parameterized and documented (95/105/115 °C tiers); ③ **Written proof of the 6 A hardware OCP's "hardware-ness"** (comparator wired to the PWM Break peripheral, immune to MCU hang, runtime watchdog + safe reset state) + S6 measured demonstration; ④ Per-voltage power-calculation regression (Aug 15–16 data), number unification (derate point 140 W / I²t 1.3 A / OV threshold aligned with acceptance spec) | S6 waveform (cut at 6 A within µs, no rupture, no demagnetization); T1–T4 all green; release to SW test Aug 18 |
| **Mid (P4 → Q4)** | ① Full production release P4 (Aug 22) with complete black box; ② Dual sensorless thermometers (R-anchored winding temperature; ψf-based magnet temperature — the software substitute for the missing winding sensor); ③ Thermal Governor closed-loop (105 °C plateau); ④ Power-on demagnetization detection (inductance-flattening / ke-drop interception, thresholds from CND's calibration map); ⑤ LKS library robustness confirmation (anti-windup, loss-of-sync detection, frozen-angle self-diagnosis) | Hot chamber 50 °C + 253 VAC: temperature plateau ≤105 °C; live interception demo on defect-seeded and demagnetized motors |
| **Long** | ① Fleet data flywheel: returned-unit fault codes + snapshots → threshold/model iteration → release evolution (rules → statistics → ML; safety actions remain rules-only forever); ② Trend-archive predictive health (per-unit R/ke drift early warning) | Quarterly threshold-iteration report; warning hit-rate |

### Luxshare R&D (system + board hardware + component sourcing — owns energy & platform)

| Horizon | Measures | Acceptance / deliverable |
|---|---|---|
| **Near** | ① Input fuse fast-blow review (M15 — mains follow-through is likely the main rupture energy after a die fails short); ② MOV presence & rating confirmation; ③ Gate-driver input pull-down confirmation (MCU reset = all six switches off, the last line of defense); ④ SPM part number and SC-withstand / Tj ratings in writing; ⑤ Acceptance-spec fix: OV trip (264 V) vs. chamber spec aligned to 253 VAC continuous + 265 V trip verification; ⑥ Singapore-specific data page (humidity mechanism + V3 status) | Board-level review report; revised acceptance protocol |
| **Mid** | ① Bus capacitors 400 V → 450 V redesign (93% voltage derating at 373 V violates the derating rule at the 260 VAC+ corner); ② **IPM Gen-1.5 introduction** (requirement spec in §3, same-footprint upgrade); ③ Gate-resistor / snubber optimization to cut du/dt overshoot (pushes transients back below the Paschen threshold — directly removes R1's ignition condition; one change, two benefits); ④ Winding-temperature channel evaluation (board revision vs. calibrated soft sensing) | Gen-1.5 sample comparison (violence benchmarked in the same destructive test); du/dt waveforms before/after |
| **Long** | ① **IPM Gen-2.0 platform** (§3: protection built into the silicon — self-protecting even with a dead MCU); ② Next-generation board platform: full derating compliance + full surge compliance (PFC cost-benefit assessment) | Platform charter + prototype |

### Luxshare Manufacturing (production line — owns cut-in & data)

| Horizon | Measures | Acceptance / deliverable |
|---|---|---|
| **Near** | ① Containment-firmware cut-in management: cut-in date + starting serial number recorded (the anchor proving "new units ≠ old units" to the customer); ② 100-unit power/current data collection (Aug 15–16, 240 V HFNH protocol); ③ EOL imbalance gate at 1.4× (**we advise against relaxing the field threshold to 1.6×** — CND's own data shows a 5% resistance deviation already reads 1.44×; 1.6 would ship marginal defects); ④ Disposition workflow for units reading >140 W in software; ⑤ Returned units processed per the three-table + surface-flux protocol, data fed back | Cut-in ledger; datasets; >140 W disposition records |
| **Mid** | ① HFNH power capture made permanent on the line; ② EOL self-test results into the per-unit archive; ③ Automated cohort tracking report (failure rate by production week — the raw material of the convergence narrative) | Weekly cohort report |
| **Long** | Full per-unit archive database (R / ke / power / self-test baselines per serial) closing the loop between factory data and returns (the production end of the data flywheel) | Database live; 100% archive coverage |

## 3. IPM Iteration Roadmap: Gen-1.5 / Gen-2.0 Requirement Specifications (reverse-derived from failure physics)

This is not "fit a bigger transistor." Every line item maps to an experimentally confirmed failure link. Baseline today: 1.5 A-class FRFET SPM, hot Rds(on) reverse-derived ≈3 Ω per die, 14× overload under fault current, hot die fails short and is then ruptured by a 9–13 J capacitor dump plus mains follow-through.

**Gen-1.5 (same footprint / pin-compatible; target 3–6 months) — "make the die survive until protection arrives":**

| # | Requirement | Anti-pop mechanism |
|---|---|---|
| 1 | Current class ≥3 A (normal peak is 2 A; today's 1.5 A rating leaves no margin) | Fault overload ratio drops 14× → 7×; die thermal-death time stretches severalfold, buying margin for the trip |
| 2 | Hot Rds(on) ≤1.5 Ω per die (half of today's ~3 Ω) | Self-heating halves → Tj drops → SC withstand does not collapse (R5 fuse defused) |
| 3 | Written SC-withstand rating: ≥5 µs at Tj = 125 °C, 373 V | Pairs with the 1.3 µs hardware trip at ≥4× margin — "the die always outlives the shutdown" |
| 4 | Datasheet Tj derating curve | The 95/105/115 °C firmware tiers get an official basis instead of empirical values |

**Gen-2.0 (platform level; target 6–12 months) — "protection in the silicon: no pop even with a dead MCU":**

| # | Requirement | Anti-pop mechanism |
|---|---|---|
| 1 | Built-in cycle-by-cycle hardware ITRIP (on-chip comparator + self-shutdown) | Locked-vector / MCU-hang / driver-latch events (R4) become immune at silicon level, independent of any external logic being alive |
| 2 | On-die temperature sensor output (analog or PWM-encoded) | Derating loop acts on true Tj, eliminating the board-NTC placement lag (today's reading is not junction temperature) |
| 3 | Miller clamp + interlock + UVLO | Closes off dV/dt spurious turn-on and half-supply misfiring (all board-side trigger candidates) |
| 4 | Failure-mode-engineered design (weak point steered to fail open + package pressure relief) | Even in failure, open-circuit is preferred over short — only a short invites the capacitor dump; paired with a fast-blow fuse this cuts the 13 J mid-path |

**Introduction gate (both generations)**: same destructive benchmark — locked-vector with protections bypassed, new IPM's failure violence/time compared against today's part; with protections enabled, zero rupture. **Luxshare R&D to obtain from the IPM vendor: roadmap, sample dates, and written commitment to each line above** (gap G11).

## 4. Meeting Evidence Exhibits (the four hard cards)

1. **T2 defect-seeded hot-chamber test** (40–50 °C + 253 VAC + CND inter-turn-shorted motors, Aug 17): zero pops — a direct reproduction of the field pop condition;
2. **S6 locked-vector immunity waveform** (production firmware): cut at 6 A within microseconds, no rupture, no demagnetization — one plot proving both that the protection is real and that the board-side path cannot complete on production units;
3. **Cohort convergence curve** (failure rate by production week, cut-in points annotated): the correct statistical lens, replacing raw weekly return counts;
4. **Singapore-specific page**: SG at 0.297% is the highest worldwide; humidity-gradient mechanism (discharge/tracking physics); V3 humidity A/B status; SG-market commitments (new batches + fast replacement policy).

Closure criteria (dual-gate version): cut-in batches ≥X thousand units × zero burnouts in the first 5 operating hours + chamber regression passed.

## 5. Information Gaps (INTERNAL ANNEX — remove before customer distribution)

| # | Gap | Why it matters | Ask |
|---|---|---|---|
| G1 | Last week's workshop minutes / closure-loop framework | Alignment of structure, owners, dates, terminology | Charles / PM |
| G2 | Singapore meeting parameters: audience, duration, language, expected output | Determines material form (working deck vs. formal bilingual commitments) | Charles |
| G3 | V1 teardown results (overdue since Aug 08) | Confidence key for R1/R2; "most probable" vs. "localized" | Quality + CND |
| G4 | Per-case ledger beyond the three tables | Case-level Q&A readiness | After-sales / Quality |
| G5 | Current SPM part number & datasheet | §3 baseline numbers must be tied to a part number | Luxshare R&D |
| G6 | Input fuse / MOV status (M15) | Reality of R6 controls | Luxshare R&D |
| G7 | V2 hi-pot ladder data (overdue) & V3 humidity A/B (Aug 15) | Real numbers for the register's verification column | Quality / Reliability |
| G8 | Cost & lead time for 450 V caps / 88 turns / temp channel / IPM Gen-1.5 | Without cost/lead-time the mid/long items are a wish list | Luxshare R&D + CND commercial |
| G9 | Weekly cohort return data by production week | Raw material for exhibit 3 | After-sales data |
| G10 | S6/T1/T2 results (Aug 17–18, right before the meeting) | Exhibits 1–2; **the test slots must be protected** | BPS + CND + Manufacturing |
| G11 | IPM vendor roadmap & sample dates for Gen-1.5/2.0 | Turns §3 from our requirements into mutual commitments | Luxshare R&D / Sourcing |
| G12 | BPS written scope confirmation of the Aug-18 build | Firmware commitments quoted in the meeting must be in writing | BPS |

## References

Doc 24 (8D, commitment baseline), Doc 26 (convergence battle plan & closure-criteria negotiation), Doc 27 (software details & acceptance matrix T1–T5), Appendix G (88-turn account), Appendix H (corner cases / capacitor derating / du:dt), Appendices J/K (evidence sources & forensic signatures), Appendix C (data flywheel).
