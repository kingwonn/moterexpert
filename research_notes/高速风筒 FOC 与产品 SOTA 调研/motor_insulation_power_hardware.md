# Motor design, winding-insulation reliability under PWM stress, and power-hardware architecture for 100–200 krpm, 100–150 W mains-powered hair-dryer motors (state as of 2026-11-13)

> **Read this first — coverage is partial.** This session hit two hard tool limits: (1) page fetching was blocked for every external host tried (nottingham-repository.worktribe.com, amsdottorato.unibo.it, patents.justia.com, mdpi.com, www.bilibili.com, reliamag.com, irispower.com, ep-ca.mersen.com), so no source could be read in full; (2) the shared web-search budget ran out after 22 queries, before any query on production screening, DC-link/fuse practice, IPM short-circuit ratings, IEC 61934/60172, or motor-supplier catalogs had run.
>
> Consequences for the reader:
> - Every externally sourced bullet below is tagged **[S]** = taken from a search-engine extract of the linked page (not full-text verified; numbers should be re-checked against the page before they go into a customer-facing document), or **[P]** = carried over from this project's own chapter-1 reference list (retrieved by the project team 2026-07-24; not re-verified here).
> - Sections 5 and 6 are almost entirely gaps. Sections 3 and 4 have qualitative support but few of the numbers that were asked for.
> - Where I recall an established figure but could not source it in this session, it is listed under **Gaps** and marked "unverified recall". Treat those as leads to confirm, not as findings.
> - All retrieval dates are 2026-11-13 unless stated.

---

## 1. Architecture of leading motors (Dyson V9 / Hyperdymium; Laifen, Dreame, Xiaomi; motor suppliers)

### Takeaway
Dyson's Supersonic motor (V9) is a 27 mm, 110,000 rpm single-phase permanent-magnet machine with a four-pole rotor between two C-shaped stator cores, run sensorless from rectified mains; the Chinese field (Laifen, Dreame, Xiaomi) is uniformly three-phase sensorless FOC on a ~310 V bus at 100,000–130,000 rpm with motor nameplate power of 60–105 W. No shipping hair dryer above 130,000 rpm was found (160–200 krpm claims trace to vacuum motors or chip-vendor reference designs), and no published slot/pole, diameter, magnet or bearing data were found for any Chinese dryer motor or for the suppliers named (CND, Kingclean, Nidec, Johnson Electric).

### Cited Findings

**Dyson V9 (Supersonic)**
- [S] Dyson states the motor spins at up to 110,000 rpm, is 27 mm wide, sits in the handle on a rubber mount, has a 13-blade impeller generating 3.5 kPa, and runs at "one inaudible frequency" (undated marketing/support pages) — [Dyson SG technology page](https://support.dyson.com.sg/haircare/supersonic/technology.aspx); [Dyson "Lean engineering"](https://www.dyson.ph/inside-dyson/corporate-social-responsibility/lean-engineering)
- [S] Electronics Weekly (April 2016) describes the Supersonic motor as a single-phase, sensorless, brushless DC permanent-magnet motor running at 115,000 rpm off rectified mains and driven by two FETs; the impeller is axial-flow with 13 blades instead of the usual 11 to push one tone above the audible range — [Electronics Weekly 2016](https://www.electronicsweekly.com/news/dyson-aims-motor-expertise-at-hair-drying-2016-04/). The 115,000 rpm figure conflicts with Dyson's own 110,000 rpm — [Dyson SG technology page](https://support.dyson.com.sg/haircare/supersonic/technology.aspx)
- [S] A former Dyson designer's portfolio page says the V9 is "assembled autonomously" and has a machined impeller spinning at 110,000 rpm in a ~27 mm motor (undated; material not named) — [Coroflot, Matt Steel](https://www.coroflot.com/mattsteel/Dyson-Supersonic-V9-Motor)
- [S] Dyson patent application US 2017/0170693 A1 "Stator assembly" (published 2017) describes a four-pole stator assembly comprising a bobbin assembly and two C-shaped stator cores, each with a back and two pole arms; the permanent-magnet rotor sits on a shaft between the two cores; pole-face asymmetry is used to increase saliency so that the rotor parks in a predictable position for restart — [Justia US20170170693](https://patents.justia.com/patent/20170170693)
- [S] US 11,038,385 "Stator assembly" (Dyson) describes a four-pole magnet interacting with the pole faces of a stator made of two C-cores — [Justia US11038385](https://patents.justia.com/patent/11038385)
- [S] US 11,588,360 "Brushless motor and stator therefor" (Dyson) claims a frame, a rotor assembly and a C-shaped stator core with a protrusion that sets the rotor clearance — [Google Patents US11588360](https://patents.google.com/patent/US11588360)
- [S] Further Dyson family members located by title only (content not read): US 11,368,058 B2 "Stator assembly" — [Google Patents](https://patents.google.com/patent/US11368058); US 9,564,778 B2 "Stator core" — [Google Patents](https://patents.google.com/patent/US9564778); US 9,627,933 "Brushless motor" — [USPTO PDF](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/9627933)
- [S] Reverse engineering of Dyson *vacuum* motors (not the V9) found single-phase brushless machines with 2 poles (DC56), 4 poles (V6) and 8 poles (V10), with a Hall-effect sensor in the control circuit — [Hackaday.io](https://hackaday.io/page/38579-motor-controller-software-and-circuit-for-dyson-vacuum-cleaner-motors). This contrasts with the sensorless description of the mains-powered V9 — [Electronics Weekly 2016](https://www.electronicsweekly.com/news/dyson-aims-motor-expertise-at-hair-drying-2016-04/)
- [S] For the earlier DDM V2 (2009, 104,000 rpm) Electronics Weekly reported a sintered neodymium rotor magnet that modelling showed was small enough not to need a carbon-fibre sleeve — [Electronics Weekly 2009](https://www.electronicsweekly.com/market-sectors/power/dyson-vacuums-104000rpm-brushless-dc-technology-2009-06/)
- [P] Motor mass 49 g (New Atlas, 2016) — [New Atlas](https://newatlas.com/dyson-supersonic-hair-dryer/43019/); a blog found this session says 46 g without a teardown basis — [blowoutiq](https://blowoutiq.com/blog/dyson-digital-motor-explained). Use 49 g.
- [P] Dyson's sensorless method (US 9,515,588 B2) compares winding voltage with an L·di/dt signal to find the back-EMF zero crossing; the parent control patent US 8,988,021 B2 covers a single-phase four-pole motor with advance/freewheel control scheduled on bus voltage and speed — [Google Patents US9515588B2](https://patents.google.com/patent/US9515588B2); [FreePatentsOnline US8988021](https://www.freepatentsonline.com/8988021.html)
- [P] Rotor construction patents: magnet retention sleeve (US 2024/0022126 A1) and balancing rings at both rotor ends (US 11,431,221 B2) — [Justia US20240022126](https://patents.justia.com/patent/20240022126); [Justia US11431221](https://patents.justia.com/patent/11431221)

**Dyson Hyperdymium**
- [S] Dyson describes Hyperdymium motors as running at up to 125,000 rpm; the 110,000 rpm figure on Dyson's site is tied to the V9, not to a "Hyperdymium" hair-dryer motor — [Dyson UK Hyperdymium page](https://www.dyson.co.uk/vacuum-cleaners/cordless/hyperdymium-motor); [Dyson AU](https://www.dyson.com.au/campaigns/dyson-hyperdymium-motor)
- [S] The "Hyperdymium 140k" motor (140,000 rpm, 28 mm) announced in 2025 is used in vacuums, not hair dryers — [Dyson Berlin launch news](https://www.dyson.com/discover/news/latest/james-dyson-unveils-problem-solving-products-in-berlin)
- [S] No source found gives a wattage, pole count or teardown for the motor in the Supersonic Nural or Supersonic r — search of Dyson pages returned marketing only — [Dyson SG newsroom](https://www.dyson.com.sg/newsroom/100m-hyperdymium-motors)
- [P] Drive electronics have changed even where the motor has not: Supersonic Nural HD15 (2024) uses a Microchip MCU with two Navitas NV6247 GaNFast half-bridges — [eefocus teardown](https://www.eefocus.com/article/1840284.html); Supersonic Travel HD19 (2026) uses a LinkoSemi MCU with three LKS1D5007DTH half-bridge modules (HV driver + 500 V / 7 A MOSFETs) — [chongdiantou teardown](https://www.chongdiantou.com/archives/1783912995960.html). Whether HD19 is three-phase is not stated in the teardown.

**Laifen**
- [S] Teardown of the Laifen SE Lite: motor nameplate reads model LF01, rated 310 V, 107,000 r/min, 105 W, manufacturer 深圳术叶创新科技有限公司 — [Bilibili teardown cv43473504](https://www.bilibili.com/read/cv43473504)
- [S] Press reports relaying Laifen's own statements say it completed its self-developed three-phase high-speed brushless motor in 2020 with a peak speed of 115,000 rpm, and that a current high-end model carries a "4th-generation" self-developed three-phase brushless motor (manufacturer claims; which of the two articles carries which statement was not verified) — [IT之家 961649](https://www.ithome.com/0/961/649.htm); [IT之家 855494](https://www.ithome.com/0/855/494.htm)
- [S] Speeds by model as reported in the trade press: SE Lite 100,000 rpm, SE 105,000 rpm, Swift 4 115,000 rpm (the search summary did not identify which of these two articles carries the list) — [36Kr](https://www.36kr.com/p/2568816853476994); [新京报](https://www.bjnews.com.cn/detail/1792492443129475.html). Note the SE Lite nameplate above says 107,000 r/min — [Bilibili teardown](https://www.bilibili.com/read/cv43473504)
- [P] Laifen Swift 4 (May 2025) adds an "integrated bearing" (shaft and bearing as one unit) — [新浪科技](https://finance.sina.com.cn/tech/digi/2025-05-23/doc-inexpvrx4005477.shtml); Laifen claims 0.001 mm part precision and 1 mg residual unbalance at its Zhuhai plant (July 2026) — [新浪科技](https://finance.sina.com.cn/tech/roll/2026-07-18/doc-iniiemqp8345293.shtml)
- [S] A third-party solution house (比派电器) advertises a 110,000 rpm dryer platform with a three-phase BLDC motor of 28 mm diameter — [什么值得买 post](https://post.smzdm.com/p/awz4n602/)

**Dreame**
- [S] Dreame Pocket folding dryer: 110,000 rpm motor (IT之家, early 2024) — [IT之家 748656](https://www.ithome.com/0/748/656.htm)
- [S] Dreame 高速 Pro (AHD4-GP) is reviewed as 130,000 rpm; G20 / G20 Pro / G10 Pro are promoted as 110,000 rpm class (consumer review and promotional posts; exact post per claim not verified) — [什么值得买](https://post.smzdm.com/p/apww6pl7/); [ZOL](https://jd.zol.com.cn/1234/12343158.html)
- [S] The 200,000 rpm Dreame motor (mass-produced 2025) is reported for vacuum cleaners; no source found puts a 160,000–200,000 rpm motor in a Dreame hair dryer (2025 consumer article; exact post not verified) — [什么值得买](https://post.smzdm.com/p/a4pz4mll/); corroborated by [P] [新浪科技 2025-04-11](https://finance.sina.com.cn/tech/roll/2025-04-11/doc-inesuxcv2519211.shtml)
- [P] Dreame G10 teardown (2024): Fortior FU6812L with three 晶艺 LAS1M0761 half-bridges, i.e. the same "MCU + three half-bridge IPM" topology as the team's product — [腾讯新闻 / 充电头网](https://news.qq.com/rain/a/20241104A082MD00)

**Xiaomi and reference designs**
- [P] Mijia H501 motor nameplate: 310 V / 60 W / 110,000 rpm, made by 东莞联峰电机; LinkoSemi LKS32MC038 + LKS1D5005DT half-bridges — [充电头网](https://www.chongdiantou.com/archives/351409.html)
- [P] Fortior's FU6812L + FD2504S reference design is specified for AC 220 V, 80 W, up to 200,000 rpm — a chip-vendor capability claim, not a shipping motor — [知乎](https://zhuanlan.zhihu.com/p/558737873)

**Market context**
- [S] QYResearch: global high-speed hair-dryer market USD 955 M in 2024 (forecast USD 3,086 M in 2031); 2024 output about 9.48 million units at an average USD 102.33 — [GII / QYResearch](https://cn.gii.tw/report/qyr1874469-high-speed-hair-dryer-global-market-share-ranking.html)
- [S] High-speed share of mainland-China dryer sales: 85% in 2025 per NielsenIQ as relayed by 北京商报 — [北京商报](https://m.bjnews.com.cn/detail/1776995627129718.html); about 70% in H1 2025 per AVC, with retail value down about 10.5% year on year (article carrying the AVC figure not identified by the search summary; one of the returned results) — [界面新闻](https://www.jiemian.com/article/13518497.html). The two figures use different bases.

### Inferences
- The team's motor (Φ27 mm, 52 g, 110,000 rpm, three-phase, 120–373 V bus) sits squarely on the Chinese mainstream template; its envelope and mass match the Dyson V9 (27 mm, 49 g) almost exactly. Nothing found suggests it is an outlier in architecture.
- Published nameplate powers span 60 W (Mijia H501) to 105 W (Laifen LF01) at 310 V. A 100–150 W electrical input, as in the team's product, is at or above the top of the published range for this frame size, which matters for the thermal findings in section 2.
- Dyson has kept the single-phase C-core motor while replacing the inverter twice (GaN in 2024, MOSFET half-bridge modules in 2026). The architecture evidence therefore does not show Dyson moving to three-phase; the HD19 topology is unresolved.
- A single-phase, bobbin-wound C-core stator keeps each coil on a moulded bobbin away from lamination edges and has no slot shared by two phases. A 6-slot three-phase concentrated winding puts coils of two different phases in every slot. This is a structural difference relevant to the in-slot failure location the team reports; it is an observation from the patent description, not a reliability claim made by any source.

### Gaps
- **Supplier specs not found**: no catalog or datasheet data for 中驱电机 (CND), 莱克 (Kingclean), Nidec, Johnson Electric or Dreame in-house dryer motors (diameter, mass, slot/pole, speed, life, insulation class). The one CND query returned only unrelated results; supplier-share data were also not found.
- **Slot/pole, magnet type, bearing type** for Laifen, Dreame and Xiaomi motors: not found in any teardown located.
- **160–200 krpm hair dryers**: no evidence of a shipping product; not found.
- **Dyson V9 internals** (magnet grade, sleeve material, bearing type and preload, lamination grade, motor input power): not published in any source located. Wikipedia's list page still describes the "Dyson Digital Motor" as switched-reluctance in one place, which applies to the 2003 X020 generation only — [Wikipedia](https://en.wikipedia.org/wiki/List_of_Dyson_products)
- Dates for the two IT之家 Laifen articles and the Bilibili teardown were not retrieved.

---

## 2. Design practice for this class (laminations, slotted vs slotless, windings, sleeves, bearings, balancing, efficiency, thermal, life)

### Takeaway
Only general design rules were retrieved: two-pole (or four-pole) rotors, thin low-loss laminations, and — at the research frontier — slotless litz windings with amorphous cores that reach about 95% machine efficiency at 100 W / 500,000 r/min. No quantitative data specific to hair-dryer motors (lamination gauge actually used, balancing grade, measured efficiency, winding temperature, life in hours) were found.

### Cited Findings
- [S] A 2025 Chinese technical report on high-speed motor design states that high-speed machines are generally 2- or 4-pole; 2-pole gives the lowest stator and field frequency and hence lowest high-frequency loss, at the cost of longer end windings; few-slot designs have larger air-gap harmonics and higher rotor eddy-current loss than many-slot designs; windings should use many thin parallel strands to limit skin and proximity loss — [OFweek, 2025-02](https://instrument.ofweek.com/2025-02/ART-8320087-8500-30656712.html)
- [S] Chinese patent CN109560677A "一种高速马达" describes a 6-slot / 2-pole high-speed blower motor with six concentrated coils (opposite teeth in the same phase); the stated aim is a symmetric stator field to reduce the unbalanced radial force on a 2-pole rotor, cutting noise and vibration and allowing a smaller outer diameter (assignee and date not retrieved) — [Google Patents CN109560677A](https://patents.google.com/patent/CN109560677A/zh)
- The four ETH Zurich items below came back as a set of abstract-index records; the search summary did not say which record is which paper, so each link is "one of the returned records", not a verified match.
- [S] ETH Zurich (Zwyssig, Kolar, Round), IEEE Trans. Industrial Electronics 2008: a 100 W, 500,000 r/min drive uses a permanent-magnet machine with a slotless litz-wire winding, giving low inductance and a high fundamental frequency — [index record](https://manuscript.isc.ac/Inventory/49/1053858.htm)
- [S] ETH Zurich 2009 efficiency-optimisation paper for the same 100 W / 500,000 r/min machine (diametrically magnetised rotor, slotless stator): loss minimisation that includes air-friction loss, together with an amorphous iron core, cuts calculated losses by 63% relative to a design that ignores air friction — [index record](https://manuscript.isc.ac/inventory/49/902929.htm)
- [S] ETH Zurich 2006 test-bench paper: the ultra-high-speed machine reaches 95% efficiency, with friction losses becoming dominant in the upper speed range — [index record](https://manuscript.isc.ac/Inventory/49/1233883.htm)
- [S] ETH "Megaspeed Drive Systems: Pushing Beyond 1 Million r/min" (2009) surveys 1 W–1 kW drives, inverter topologies suited to very low stator inductance, and bearing options (ball, air, foil, magnetic) — [index record](https://search.isc.ac/inventory/49/3305159.htm); press coverage describes ultra-thin copper windings inserted into a cylinder of "special iron" — [phys.org 2008](https://phys.org/news/2008-11-world.pdf)
- [S] A CSDN blog states that high-speed PM stators commonly use 0.2 mm low-loss silicon steel; low-quality source — [CSDN](https://blog.csdn.net/weixin_43260261/article/details/148780852)
- [P] Lamination guidance used in this project: above 400 Hz use ≤0.2 mm or 0.1 mm laminations — [Ruixin](https://www.ruixinmould.com/news/industry-news/motor-stator-laminations-materials-thickness-manufacturing.html); 6.5% Si steel 10JNEX900 versus amorphous 2605SA1 comparison — [PMC9736568](https://pmc.ncbi.nlm.nih.gov/articles/PMC9736568/)
- [P] Bearings: grease-lubricated deep-groove ball bearings have a guideline limit of about 250,000 DN — [Bearing Works](https://www.bearingworks.com/technical-data/maximum-speed.php); dental-handpiece bearings of 3 mm bore run above 300,000 rpm — [NSK 2021](https://www.nsk.com/eu-en/company/news/2021/miniature-precision-bearings-for-dental-drills/)
- [P] Dreame states its dryer motor operating temperature is held at about 110 °C (2022) — [投资家网](http://www.investorscn.com/2022/04/14/100215/)
- [P] Rotor sleeve material and stress review — [KIT](https://publikationen.bibliothek.kit.edu/1000125982/128560422)

### Inferences
- The ETH results are for slotless machines at 5× the team's speed and are not a like-for-like benchmark; they do show that at this power level windage and bearing friction, not electromagnetic loss, set the efficiency ceiling once iron and copper loss are designed down. A slotted 6-slot dryer motor on silicon steel should be expected to sit well below 95%.
- For a 2-pole rotor at 110,000 rpm the fundamental is 1.83 kHz. Copper skin depth at that frequency is about 1.5 mm and about 0.47 mm at 20 kHz (computed from copper resistivity), both far above the 0.08 mm radius of 0.16 mm wire, so strand subdivision is not needed for skin effect in this winding; any high-frequency copper loss is proximity loss from slot leakage flux.
- Dreame's ~110 °C operating point and the team's own 110–120 °C winding estimates are both close to the Class B limit of 130 °C once hot-spot allowance is included. No source found shows a dryer-motor maker publishing a higher insulation class.

### Gaps
- **Not found**: lamination thickness and grade actually used in production dryer motors; use of amorphous or 0.1 mm steel in this product class; slotless designs in shipping dryers.
- **Not found**: balancing grade (ISO 21940 G-value) practice; bearing type (hybrid ceramic or steel), preload force and grease; rotor sleeve practice for ring magnets on ~6 mm rotors.
- **Not found**: measured efficiency, power density, winding or magnet temperature, or rated life in hours for any hair-dryer motor from a primary source.
- The ETH records above were returned as index pages; the mapping of each URL to each paper was not verified because the pages could not be opened.

---

## 3. Insulation under inverter-fed operation at ≤400 V bus (PDIV of thin enamelled wire, environmental effects, Paschen arguments, plausibility at 311–373 V, lifetime once PD is present)

### Takeaway
The literature found supports the general mechanism — partial discharge in air gaps next to thin enamel is the recognised failure mode of low-voltage random-wound machines on PWM supplies, PDIV falls with temperature, humidity and enamel thinning, and ordinary enamel does not survive sustained PD — but no source was found that reports PD, or a measured PDIV, for 0.1–0.2 mm wire or for a ≤400 V-bus appliance motor. On the numbers available, PD at 311–373 V is plausible only between conductors that see the full bus step with overshoot across thin or damaged enamel; a single weak point between one winding and a truly floating core sees at most two-thirds of the bus and should not ignite on its own.

### Cited Findings

**Mechanism in low-voltage random-wound machines**
- [S] Madonna et al., "On the design of partial discharge free low voltage electrical machines" (IEEE IEMDC 2019, Nottingham): PD inception is called one of the most critical reliability factors "in particular for low voltage, random wound machines"; the design method combines Paschen's law for air at sea-level pressure with an electrostatic finite-element model of the turn-to-turn insulation, validated by PD-inception measurements on purpose-built prototypes — [Nottingham repository](https://nottingham-repository.worktribe.com/output/2441889/on-the-design-of-partial-discharge-free-low-voltage-electrical-machines)
- [S] Kaufhold, Auinger, Berth, Speck, Eberhardt, "Electrical stress and failure mechanism of the winding insulation in PWM-inverter-fed low-voltage induction motors", IEEE Trans. Industrial Electronics 47(2), 396–402 (2000). Secondary summaries: the non-linear voltage distribution along the winding favours PD, which in low-voltage machines causes excessive insulation damage; PD inception voltage and the speed of the resulting electrical ageing depend on voltage waveform, insulation design and temperature — [ResearchGate record](https://researchgate.net/publication/3217652_Electrical_stress_and_failure_mechanism_of_the_winding_insulation_in_PWM-inverter-fed_low-voltage_induction_motors); [Semantic Scholar companion paper](https://www.semanticscholar.org/paper/Failure-mechanism-of-the-interturn-insulation-of-by-Kaufhold/5be07eca8cd00cc422fb3301dcc6281b8f363b11)
- [S] Practitioner summary: in standard low-voltage motors PDIV is much higher than the operating voltage; PD arises when high bus voltage, fast rise times and high pulse rates create transients that exceed PDIV — [Reliamag](https://reliamag.com/topics/articles/partial-discharge-vfd-motors)

**PDIV versus enamel thickness (Dakin-type empirical formula)**
- [S] Wire-maker patents use V = 163·(t/εr)^0.46, with V the PDIV, t the insulation thickness in µm and εr the relative permittivity; one patent writes V = √2 × 163 × (t/εr)^0.46 in V peak, with t the insulation thickness between the conductors of a twisted pair — [USPTO US10418151](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/10418151); [magnet-wire PDIV application](https://trea.com/information/magent-wire-with-high-partial-discharge-inception-voltage-pdiv/patentapplication/f61ebff2-d9a5-4078-8a6d-6465bd152220)
- [S] Sumitomo Electric's technical review writes the same relation as V = √2 × 163 × (2t/εr)^0.46, i.e. with twice the single-wire film thickness inside the bracket — [SEI Technical Review E88-10](https://global-sei.com/technology/tr/bn88/pdf/E88-10.pdf)
- [S] The measurement behind these figures: twisted pair of two insulated wires, 50 Hz sine voltage raised at a uniform rate, PDIV read at a 10 pC threshold, then normalised with the formula — [USPTO US10020092](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/10020092)
- [S] A 2023 comparison of PDIV models lists the Dakin relation as using only εr and thickness, independent of enamel material and wire diameter, and valid at room conditions; the same paper compares measured and predicted PDIV at several temperatures — [Applied Sciences 13(4):2417, 2023](https://doi.org/10.3390/app13042417)

**Temperature and ageing**
- [S] Rumi, Lusuardi, Cavallini et al., "Partial Discharges in Electrical Machines for the More Electrical Aircraft. Part III: Preventing Partial Discharges" (2021): a search extract gives a PDIV drop of about 9% from room temperature to 140 °C (wire type and statistic not shown in the extract — unverified); impregnating resins raise PDIV at room temperature but can degrade at high temperature, with one impregnated pair at 140 °C falling below the bare, non-impregnated wire; during thermal ageing at 230 °C both enamel thickness and twisted-pair PDIV fall — [ResearchGate record](https://www.researchgate.net/publication/349190221_Partial_Discharges_in_Electrical_Machines_for_the_More_Electrical_Aircraft_Part_III_Preventing_Partial_Discharges); [figure](https://www.researchgate.net/figure/PDIV-of-twisted-pairs-during-thermal-aging-at-230C-The-plot-shows-the-average-values_fig2_349190221)
- [S] A 2024 review tabulates twisted-pair PDIV studies across 25–180 °C together with pressure, humidity, frequency and rise-time effects (table values not retrieved) — [Energies 17(16):3980, 2024](https://www.mdpi.com/1996-1073/17/16/3980)
- [S] A 2024 study tracks PDIV of enamelled-wire twisted pairs over time under thermal stress (values not retrieved) — [Energies 17(18):4578, 2024](https://doi.org/10.3390/en17184578)

**Humidity**
- [S] Kikuchi et al. (2008), conventional and nanocomposite enamelled wires: PDIV decreased with rising relative humidity at low temperature, increased slightly with humidity at high temperature, and showed a humidity level of minimum PDIV — [catalogue record](https://katalog.lib.cas.cz/KNAV/EdsRecord/edsair,edsair.doi...........448b0a96b5d308c86549a70d4d5438bd)
- [S] Same group, nanocomposite enamelled wire for inverter-fed random-wound motors: at 95% RH the PDIV of conventional enamel dropped sharply while the nanocomposite wire dropped much less — [catalogue record](https://katalog.lib.cas.cz/KNAV/EdsRecord/edsair,edsair.doi...........f820442a16abe0ea92291d6018bcbb5d); the changes are attributed to permittivity increase from moisture absorption and to surface wettability under voltage — [catalogue record](https://searchworks.jioinstitute.edu.in/articles/edsggo__edsgcl.190331927)
- [S] Kikuchi et al. (2010), optical emission spectroscopy on twisted enamelled wires in humid air: field strength in the gap rose with humidity at a temperature-dependent rate, which the authors relate to a lower PDIV; at high temperature discharge current fell with humidity, attributed to electron attachment to water molecules — [record](https://search.isc.ac/inventory/49/1522696.htm)
- [S] Under repetitive impulses, the repetitive PDIV (RPDIV) of twisted enamelled wires under bipolar impulses was scattered at the start in dry air but stabilised quickly in humid air; unipolar RPDIV showed no scatter; the fluctuation is linked to surface charge left by earlier discharges — [CiNii record](https://cir.nii.ac.jp/crid/1390282679575836160)
- [S] A dedicated paper exists on the combined effect of temperature and humidity on twisted-pair PDIV (Rumi, Cavallini, Lusuardi); its numbers were not retrieved — [ResearchGate record](https://www.researchgate.net/publication/349124028_Combined_Effects_of_Temperature_and_Humidity_on_the_PDIV_of_Twisted_Pairs)
- [S] IEC 60034-18-41 Amendment 1 (2019) recognises the effect of relative humidity and of altitude (air density) on PD inception voltage and suggests enhancement factors for testing — [iTeh listing](https://standards.iteh.ai/catalog/standards/iec/4be135d3-d173-467b-b078-8165a78c2b08/iec-60034-18-41-2014-amd1-2019)

**Rise time and pulse statistics**
- [S] Hayakawa and Okubo (IEEE, 2012) applied volume–time theory to PDIV of magnet-wire twisted pairs under inverter surge voltage: inception depends on the probability of an initial electron appearing in the high-field volume during the short surge; the estimated PDIV tracked measurements and fell as the rise time of the applied waveform increased (i.e. shorter rise time → higher PDIV) — [record](https://search.isc.ac/inventory/49/1490895.htm)

**Lifetime once PD is present**
- [S] RWTH Aachen IEM, Archives of Electrical Engineering 69(1), 187–202 (2020): twisted-pair life expectancy is reported in hours — 221 h for a Grade 2 PD-resistant wire and more than 500 h for a Grade 3 corona-resistant wire; a standard wire was also tested (its figure and the test voltage/frequency were not in the extract) — [RWTH PDF](http://bib.iem.rwth-aachen.de/IEMpublications/2020FPLow.pdf)
- [S] CIGRE WG D1.43 technical brochure "Insulation degradation under fast, repetitive voltage pulses": a nanocomposite enamelled-wire twisted pair lasted 16 times longer than conventional enamel at 60 Hz, 2 kV, 150 °C, and more than 1,000 times longer at 1.13 kV; the brochure also relays the suggestion that the average number of cycles to breakdown is independent of frequency — [CIGRE TB PDF](https://cigre.cz/dokumenty_komise/d1/WG%20D1.43_TB_Final.pdf)
- [S] Life tests on twisted pairs in the presence of PD show that useful life is significantly influenced by frequency and rise time of the waveform — [ResearchGate record](https://www.researchgate.net/publication/254062767_Life_Tests_on_Twisted_Pairs_in_Presence_of_Partial_Discharges_Influence_of_the_Voltage_Waveform); a 2023 breakdown study covers twisted pairs under high-switching-frequency PWM and square waveforms — [ResearchGate record](https://www.researchgate.net/publication/372788578_Breakdown_tests_on_Twisted_Pairs_subjected_to_high_switching_frequency_of_PWM_and_Square_waveforms)
- [S] Under PD from fast repetitive pulses, corona-resistant wires outperform non-corona-resistant wires — [ResearchGate record](https://www.researchgate.net/publication/3340802_Evaluation_of_medium_voltage_enameled_wire_exposed_to_fast_repetitive_voltage_pulses)

### Inferences

**I-1. What the Dakin relation gives for thin enamel (room conditions, my calculation from the cited formula in its V-peak form).** t is the total enamel thickness between the two conductors.

| t between conductors (µm) | εr = 3.5 | εr = 4.0 | εr = 4.5 |
|---|---|---|---|
| 6 | 295 V | 278 V | 263 V |
| 8 | 337 V | 317 V | 300 V |
| 10 | 374 V | 351 V | 333 V |
| 12 | 406 V | 382 V | 362 V |
| 15 | 450 V | 423 V | 401 V |
| 20 | 514 V | 483 V | 458 V |
| 25 | 569 V | 536 V | 507 V |
| 30 | 619 V | 582 V | 552 V |
| 40 | 707 V | 665 V | 630 V |

Values are peak volts. Entries below roughly 330 V are below the Paschen floor for air that the project uses ([Appendix J](../../docs/23-附录J-证据台账与根因重推.md)) and should be read as "floor-limited", not as real PDIV. The formula is an empirical fit at room conditions and its validity for films this thin was not confirmed by any source found. The minimum enamel build for 0.16 mm Grade 1 / Grade 2 wire was not retrieved, so the row that applies to the team's wire must be picked from the wire specification.

**I-2. Stress available versus threshold.** Bus voltage is 311 V at 220 VAC, 339 V at 240 VAC and 373 V at 264 VAC. With 10–30% overshoot the phase-to-phase step is 342–404 V, 373–441 V and 410–485 V respectively. Low-line buses (141–170 V) give at most 183–221 V even with 30% overshoot.
- Two touching wires of different phases with thin enamel (t in the 12–20 µm range): the table gives about 380–510 V at room conditions. A 373 V bus with overshoot overlaps this range; a 311 V bus needs both thin enamel and high overshoot. Applying the cited ~9% reduction at 140 °C and the cited humidity reduction at low temperature moves the threshold further into reach. **This supports the hypothesis for phase-to-phase contacts.**
- A wire whose enamel is damaged, so that only one film or a bare spot faces another conductor, drops to the floor-limited rows: any step above roughly 330 V could ignite it. **This supports the hypothesis for defect sites that see the full bus.**
- Low line can never reach the floor. **This is consistent with the 17× voltage gate in the field data.**

**I-3. A single wire-to-core weak point does not see the full bus if the core floats.** If the stator core is isolated and coupled about equally to the three phases, its potential follows the common-mode voltage, so the largest instantaneous winding-to-core voltage is two-thirds of the bus: 207 V at 311 V and 249 V at 373 V, or 323 V with 30% overshoot at 373 V. That is at or below the ~330 V floor. Two weak points in different phases with a floating core between them put two gaps in series, which is harder still to ignite. **This qualifies the in-slot wire-to-core hypothesis**: for gas discharge to start at a wire-to-core defect, something must pin the core — a first defect that is already a conductive contact to one phase, a moisture or contamination film, or a low-impedance tie between core/housing and a DC rail. Once the core is tied to one phase, a weak point on the other phase's coil in the same slot sees the full bus step and I-2 applies. The team's finding of 9 of 18 returned motors with winding-to-housing resistance ≤360 kΩ ([Appendix J §J.8](../../docs/23-附录J-证据台账与根因重推.md)) fits a "first defect is a conductive contact, second defect takes the full step" sequence. Measuring the core-to-DC-minus waveform on a healthy motor would settle whether the core floats at common mode.

**I-4. Fast edges do not make inception easier per pulse.** Under volume–time theory a short rise time raises the measured inception voltage because a starting electron may not be available during the pulse. Sinusoidal 50 Hz PDIV values (the basis of the Dakin fit) are therefore, if anything, optimistic about how readily a single PWM edge ignites; what PWM adds is the number of attempts per second and the overshoot. **This mildly contradicts a simple "above 330 V equals PD on every edge" picture.**

**I-5. Lifetime.** The sources found agree on direction (ordinary enamel fails fast under sustained PD; PD-resistant and corona-resistant builds last far longer) but give lifetimes only for PD-resistant wires or for test voltages of 1.1–2 kV. No source found gives pulses-to-failure for standard enamel a few tens of volts above PDIV. The project's working figure of 10⁹–10¹⁰ pulses, hence 3–30 h, is not confirmed or contradicted by anything retrieved here.

**I-6. Balance of evidence on "PD is plausible at 311–373 V bus".**
- Supports: the recognised mechanism in LV random-wound machines; Dakin-relation thresholds of roughly 380–510 V for thin two-film contacts and floor-limited values for damaged film; PDIV reductions with temperature, humidity and enamel thinning; a hard lower bound that excludes low line.
- Contradicts or qualifies: practitioner literature holds that PDIV normally sits well above operating voltage in LV motors and that exceeding it needs transients; the main overshoot source in the literature (cable reflection) is absent when the inverter is centimetres from the motor; a floating core limits wire-to-core stress to two-thirds of the bus; shorter rise time raises inception voltage; no published case of PD-driven failure in a ≤400 V-bus appliance motor was found.
- Net: plausible for phase-to-phase or pinned-core defect sites at high line when hot and humid; not established for intact insulation; not established for an isolated wire-to-core weak point with a floating core.

### Gaps
- **Measured PDIV for 0.1–0.2 mm wire, Grade 1/2/3**: not found. The queries aimed at fine-wire PDIV returned nothing usable before the search budget ran out.
- **Wire-to-core PDIV through slot liner or bobbin, with voids or damaged enamel**: not found; the query did not run.
- **Paschen minimum**: no external source retrieved in this session. Unverified recall: about 327 V at a pressure–distance product of about 0.57 Torr·cm (roughly 7.5 µm at 1 atm). The project uses ~330 V. Confirm against a primary reference before quoting.
- **Altitude and frequency effects on PDIV**: only the fact that the 2024 Energies review and IEC 60034-18-41 Amd 1 address them; no numbers retrieved.
- **Measured overshoot for board-mounted inverters with no motor cable**: not found.
- **Pulses to failure for standard enamel marginally above PDIV**: not found (see I-5).
- **Non-PD humid-environment mechanisms** (surface tracking across bobbin or slot liner, enamel hydrolysis, electrochemical migration) were not researched; the humidity gradient in the field data is also what those mechanisms would produce, so they are not excluded by anything here.
- Full texts that would close most of this section were located but could not be opened: Madonna 2019, the Lusuardi doctoral thesis ([Bologna](https://amsdottorato.unibo.it/id/eprint/9272/1/Lusuardi_Luca_tesi.pdf)), the 2024 Energies review, Applied Sciences 2023, the RWTH 2020 paper, the CIGRE D1.43 brochure, and a Nagoya University report ([PDF](https://nagoya.repo.nii.ac.jp/record/18638/files/12.pdf)).

---

## 4. Standards and qualification (IEC 60034-18-41/-42, IEC 61934, IEC 60172, IEC 60317 / GB/T 6109; corona-resistant wire; impregnation; first-coil stress)

### Takeaway
IEC 60034-18-41 (2014, amended 2019, with a 2025 draft revision in circulation) is the governing framework for "Type I" PD-free low-voltage insulation and now accounts for humidity and altitude, and IEC 60317-0-1 (latest consolidated edition February 2026) holds the continuity and pin-hole limits for enamelled wire — but none of the numeric tables (stress-category overshoot factors, enhancement factors, pin-hole counts, test voltages) could be retrieved, and nothing was retrieved on IEC 61934, IEC 60172, -18-42, or impregnation practice for miniature stators.

### Cited Findings
- [S] IEC 60034-18-41 Edition 1.0 (2014-03) contains a Type I test procedure, a clause on voltages for design-qualification and type tests, and an annex on enhancement factors (B.3); it replaced the 2006 Technical Specification — [VDE preview Ed.1.0](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec60034-18-41{ed1.0}b.pdf); [SAI Global TS 2006 record](https://infostore.saiglobal.com/en-us/standards/iec-ts-60034-18-41-2006-564092_saig_iec_iec_1286716/)
- [S] Amendment 1 (consolidated Edition 1.1, 2019-06) reorganises type and routine tests, adds an annex deriving allowable service voltages from an impulse voltage insulation class, updates test-voltage factors with worked examples for 500 V machines, and recognises relative humidity and altitude effects on PDIV with suggested enhancement factors — [VDE preview Ed.1.1](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec60034-18-41{ed1.1}b.pdf); [iTeh Amd 1 listing](https://standards.iteh.ai/catalog/standards/iec/4be135d3-d173-467b-b078-8165a78c2b08/iec-60034-18-41-2014-amd1-2019)
- [S] A draft new edition, prEN IEC 60034-18-41:2025, is listed — [iTeh prEN listing](https://standards.iteh.ai/catalog/standards/clc/ff53be25-ba84-4720-bce9-a6350f127919/pren-iec-60034-18-41-2025)
- [S] The standard classifies converter stress by impulse magnitude and rise time into four categories — benign, moderate, severe, extreme — with severity rising as rise time shortens (secondary article; which of the two returned articles carried this was not verified) — [Iris Power paper](https://irispower.com/wp-content/uploads/2018/06/IEC-Standards-for-Variable-Speed-Drives-and-Motor-Winding-Insulation.pdf); [IA Magazine reprint](https://www.marubun.co.jp/wp-content/uploads/a7ijkd000000119x/ia-mag-07.pdf)
- [S] One of those two older articles quotes a qualification impulse of 3.1 times rated phase-to-phase voltage; edition and context were not visible, so treat as unverified — [IA Magazine reprint](https://www.marubun.co.jp/wp-content/uploads/a7ijkd000000119x/ia-mag-07.pdf)
- [S] IEC 60317-0-1 (enamelled round copper wire, general requirements): clause 14 covers continuity of insulation (limits in Table 13) and clause 23 the pin-hole test (maximum pin holes in Table 14); consolidated Edition 4.1 is dated 2019-08 and a consolidated Edition 4.2 is dated 2026-02 — [IEC preview via technickenormy.cz](https://www.technickenormy.cz/publicdoc/iec_previews/4313004.pdf); the 1997 second edition already had a breakdown-voltage clause (clause 13) — [VDE preview Ed.2.0](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec60317-0-1{ed2.0}b.pdf)
- [S] Test methods (IEC 60851-5): continuity faults are counted individually and reported per 30 m of wire; the pin-hole test is a visual examination after a defined conditioning sequence — [Lectromec](https://lectromec.com/winding-wires-electrical-property-test-methods/)
- [S] Corona-/PD-resistant wire evidence: PD-resistant Grade 2 and corona-resistant Grade 3 twisted pairs lasted 221 h and >500 h in the RWTH test — [RWTH 2020](http://bib.iem.rwth-aachen.de/IEMpublications/2020FPLow.pdf); nanocomposite enamel lasted 16× to >1,000× longer than conventional enamel depending on voltage — [CIGRE WG D1.43](https://cigre.cz/dokumenty_komise/d1/WG%20D1.43_TB_Final.pdf); nanocomposite enamel also lost much less PDIV at 95% RH than conventional enamel — [Kikuchi et al. record](https://katalog.lib.cas.cz/KNAV/EdsRecord/edsair,edsair.doi...........f820442a16abe0ea92291d6018bcbb5d); a patent on enamelled wire with high PD resistance was located by title only — [USPTO US6087592](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6087592)
- [S] Impregnation is not automatically protective: resin raises PDIV at room temperature but one impregnated twisted pair fell below the unimpregnated wire at 140 °C — [Rumi et al., Part III](https://www.researchgate.net/publication/349190221_Partial_Discharges_in_Electrical_Machines_for_the_More_Electrical_Aircraft_Part_III_Preventing_Partial_Discharges)
- [S] Voltage distribution: under fast-fronted PWM the overvoltage is unevenly distributed and turn-to-turn stress is concentrated in the first few turns — [InTechOpen chapter](https://www.intechopen.com/chapters/70354); a UK industry guide on motor insulation voltage stress under PWM supply was located but not read — [GAMBICA guide](https://www.gambica.org.uk/static/uploaded/e41820bb-886a-463b-829aaa07c6ec486c.pdf)
- [S] A Korean study evaluates stator-winding insulation of inverter-fed induction motors by impregnation technique (title only) — [KoreaScience](https://koreascience.kr/article/JAKO200625121592097.page)

### Inferences
- Because the 2019 amendment builds humidity and altitude into Type I qualification, a PD-free demonstration done only at room conditions and sea level would not meet the current intent of the standard; qualification of the team's motor should be run hot and humid with the corresponding enhancement factors.
- The Part III result means "add varnish" needs to be verified at operating temperature with the actual resin; a resin that softens or debonds at 120–140 °C can leave voids that lower PDIV.
- For a motor whose inverter is on the same board, cable-reflection overshoot (the basis of the stress categories) is small, so the stress category is set mainly by switching-loop ringing and measured dv/dt. First-coil concentration still applies in principle, but each phase here has only two series coils of about 85 turns, so the distribution problem differs from the multi-coil industrial windings the literature addresses; no source found covers this case.

### Gaps
- **Not found**: numeric overshoot factors per stress category, enhancement-factor values, and Type I test voltages — the standard is paywalled and the previews show contents pages only.
- **Scope question — unverified recall**: IEC 60034-18-41 is, as I recall it, stated not to apply to machines with rated voltage of 300 V r.m.s. or below. If confirmed, the team's motor is formally outside its scope and the standard can only be used as a method reference. This must be checked in clause 1 of the current edition.
- **Not found**: anything on IEC 60034-18-42 (Type II), IEC 61934 (PD measurement under repetitive impulses; RPDIV definition) or IEC 60172 (thermal endurance of enamelled wire) — the queries did not run.
- **Not found**: the IEC 60317-0-1 / GB/T 6109 limits themselves (faults per 30 m by grade and diameter, continuity test voltage, minimum breakdown voltage and minimum enamel build for 0.16 mm wire). The project's working figures (Grade 1 ≤ ~15 per 30 m, Grade 2 ≤ 5 per 30 m, in [Appendix I](../../docs/22-附录I-线材缺陷根因分析.md)) remain unverified against the standard.
- **Not found**: magnet-wire maker notes (Essex, Elektrisola, 精达, 冠城大通); commercial corona-resistant fine-wire availability at 0.16 mm; impregnation, potting or over-moulding practice for stators of this size; dv/dt filter or gate-resistance guidance specific to appliance IPMs.

---

## 5. Production screening for small motors (surge with PD detection, hi-pot levels, typical voltages for 230 V appliance motors, equipment vendors, detection capability)

### Takeaway
Nothing was retrieved for this question: the search budget was exhausted before any screening query ran. The only relevant data points are the project's own.

### Cited Findings
- No external findings. No query on surge/PD test equipment (Schleich, Megger/Baker, 艾普 AIP, 艾诺 Ainuo), hi-pot levels or defect-detection capability returned results in this session.
- [P] Project data, for context only: the motor's winding-to-housing hi-pot specification is 1,600 VAC; one returned unit passed 500 V and broke down at 800 V; 9 of 18 returned units had winding-to-housing resistance ≤360 kΩ — [Appendix J §J.8](../../docs/23-附录J-证据台账与根因重推.md)

### Inferences
- From section 3: a screen meant to catch the hypothesised weak points must stress wire-to-core and phase-to-phase at well above the Paschen floor and detect discharge, not only breakdown. A 50 Hz hi-pot that merely looks for leakage current above a trip threshold will pass a pin-hole whose air gap does not break down at the test voltage. This follows from the mechanism; no vendor capability claim was retrieved to back it.

### Gaps
- **Not found**: typical surge-test voltages and PD-detection thresholds for 230 V-class appliance stators; routine hi-pot values required by IEC 60335-1 or IEC 60034-1 for this construction.
- **Not found**: vendor specifications or application notes from Schleich, Megger/Baker, AIP or Ainuo; any published detection rates for pin-holes, nicked wire or weak slot insulation; reduced-pressure ("vacuum") weak-point tests used on traction stators and whether they scale to Φ27 mm stators.
- **Not found**: any published end-of-line practice from dryer-motor makers.

---

## 6. Power architecture in universal-voltage products (PFC, voltage doubler vs wide-range bus, DC-link sizing and derating, inrush, fuses and I²t coordination, MOV/surge)

### Takeaway
Teardown evidence shows mainstream high-speed dryers rectify the mains straight onto the inverter bus with no PFC stage and feed the heater separately through a TRIAC; no source was retrieved on voltage-doubler use, DC-link derating rules, inrush limiting, fuse selection or MOV practice in this product class.

### Cited Findings
- [P] Mainstream architecture from teardowns: fuse and EMI filter → bridge rectifier and electrolytic bulk capacitor → ~310 V bus → three-phase inverter; non-isolated buck to ~15 V then LDO to logic; heater driven directly from the mains through a TRIAC (e.g. BTB16-800BW). No PFC stage appears in the teardowns cited — [Mijia H501](https://www.chongdiantou.com/archives/351409.html); [Dreame G10](https://news.qq.com/rain/a/20241104A082MD00); [Laifen SE 02](https://www.chongdiantou.com/archives/382523.html)
- [P] Motors are wound for the high-voltage bus: nameplates read 310 V (Mijia H501, 60 W) — [充电头网](https://www.chongdiantou.com/archives/351409.html); [S] and 310 V (Laifen LF01, 105 W) — [Bilibili teardown](https://www.bilibili.com/read/cv43473504)
- [P] An aftermarket Dyson V9 motor-and-board assembly is listed with a 140–180 VDC bus, matching 100–120 V markets; low-confidence third-party listing — [AliExpress listing](https://www.aliexpress.com/item/1005006256110961.html)
- [S] IEC 61000-3-2 permits asymmetrical control and half-wave rectification directly on the mains only in limited cases, one of which is portable equipment with a two-core flexible cord intended for use for a few minutes; hair dryers are named as an example. The text came from a third-party reproduction of the 2014 edition among the search results (the summary did not say which); carry-over into Edition 5 (2018) was not confirmed — [CSDN文库 copy](https://wenku.csdn.net/doc/6m25o6bj48); [Wikipedia](https://en.wikipedia.org/wiki/IEC_61000-3-2); [VDE preview Ed.5.1](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec61000-3-2{ed5.1}b.pdf)
- [P] Project schematic data, for context: bus 120–373 VDC direct from the rectifier; bulk capacitance 4 × 47 µF / 400 V = 188 µF — [Appendix J §J.7](../../docs/23-附录J-证据台账与根因重推.md)

### Inferences
- Stored bus energy with 188 µF is 1.9 J at 141 V, 9.1 J at 311 V and 13.1 J at 373 V (½CV²) — a seven-fold spread between low and high line, which matters for section 7.
- At 264 VAC the bus is 373 V, which is 93% of a 400 V capacitor rating before any surge.
- Architecture options and their effect on insulation stress follow from the bus voltage alone: a low-line voltage doubler would narrow the bus range to roughly 280–373 V and so raise low-line stress to high-line levels rather than reduce anything; a boost PFC would fix the bus near 390–400 V for every market; only a step-down pre-regulator, or region-specific windings on separate SKUs, would keep the motor bus below the Paschen floor. None of these is documented in a dryer teardown found here; they are design-space reasoning, not observed practice.
- Evidence that region-specific builds exist is weak but present: the 140–180 VDC Dyson assembly listing suggests Dyson ships a low-bus variant for 100–120 V markets rather than one universal winding.

### Gaps
- **Not found**: any source on PFC use, voltage-doubler rectifiers or universal-input strategy in hair dryers or comparable 100–150 W motor appliances.
- **Not found**: DC-link capacitor sizing rules, voltage-derating guidance (the 80–90% rule used in the project is unverified here), ripple-current and lifetime data.
- **Not found**: inrush limiting practice (NTC versus resistor-plus-bypass), fuse type and rating practice, fuse I²t coordination with bulk-capacitor energy, MOV selection and surge levels for this class.
- **Not found**: quantified effect of bus level or bus ripple on motor ripple loss from an external source (the project has its own measurements).
- The harmonic class and limits that apply to the motor-drive rectifier of a dryer under IEC 61000-3-2 were not retrieved; only the asymmetrical-control clause was.

---

## 7. Power devices (IPM vs discrete vs GaN; short-circuit withstand vs junction temperature; integrated protection; package rupture from DC-link discharge and prevention)

### Takeaway
Shipping dryers use 500 V-class MOSFET half-bridge or three-phase IPMs almost universally, with GaN half-bridges appearing only in Dyson's 2024 Nural; the rupture literature — all of it on large IGBT modules — agrees that the DC-link capacitor is the energy source, that discharge happens in tens of microseconds through very low inductance, and that the classical remedy is a fast fuse whose total let-through I²t is below the package's rupture I²t. No short-circuit-withstand or rupture data for small appliance IPMs was retrieved.

### Cited Findings
- [P] Device choices seen in teardowns: three half-bridge IPMs (Dreame G10: 晶艺 LAS1M0761 ×3; Mijia H501: LinkoSemi LKS1D5005DT; Laifen SE Lite: 亿晶微 EG27710) or one three-phase 500 V IPM (Laifen SE 02: 吉林华微 SPE05M50T-C) — [Dreame G10](https://news.qq.com/rain/a/20241104A082MD00); [Mijia H501](https://www.chongdiantou.com/archives/351409.html); [Laifen SE Lite](https://www.chongdiantou.com/archives/337441.html); [Laifen SE 02](https://www.chongdiantou.com/archives/382523.html)
- [P] GaN in this class: Dyson Supersonic Nural HD15 (2024) uses two Navitas NV6247 GaNFast half-bridge ICs — [eefocus](https://www.eefocus.com/article/1840284.html); Dyson's 2026 HD19 reverts to silicon, three LKS1D5007DTH modules with 500 V / 7 A MOSFETs — [充电头网](https://www.chongdiantou.com/archives/1783912995960.html)
- [S] A 1997 IEEE study characterised IGBT module rupture by test and gave two separate ratings per device, because over-current failure and unclamped-voltage failure give different rupture energy and I²t; it notes that a DC-link bus fuse was being adopted in most designs to limit extreme fault currents and module rupture — [record](https://manuscript.isc.ac/Inventory/49/316905.htm)
- [S] Explosion tests on high-voltage IGBT modules: the prime energy source is the DC-link capacitor, and because parasitic inductance is very low the characteristic discharge time is only 30–100 (unit garbled in the extract, evidently microseconds) — [typeset.io PDF](https://typeset.io/pdf/explosion-tests-on-igbt-high-voltage-modules-3af9pz53ch.pdf)
- [S] Fuse-maker criterion: the fuse's total let-through I²t (pre-arcing plus arcing) must stay below the level that damages the protected part, which for IGBTs is the I²t that causes case rupture — [Mersen article](https://ep-ca.mersen.com/sites/mersen_ca/files/2018-11/AR-Fast-Acting-Fuses-For-Semiconductor-Protection-Article.pdf); [Mersen tech topic](https://ep-ca.mersen.com/sites/mersen_ca/files/2018-11/TT-PEN3-Fast-Acting-Fuses-for-Semiconductor-Protection-Tech-Topic.pdf)
- [S] Vendor example for a large module: a 400 A fast fuse letting through about 37,000 A²s damaged the IGBT housing while an ultra-fast fuse at about 27,500 A²s did not; about 30,000 A²s was used as the case-rupture figure for selection. Not transferable to small IPMs — [fuse vendor note](https://www.asbeam.com/news/ultra_fast_acting_fuse_IGBT_2-cn.html)
- [S] Aalborg University's low-inductance IGBT fuse project notes that a high-power IGBT retains a small risk of violent rupture even with active protection, motivating high-speed fuses in the DC link — [Aalborg project page](https://vbn.aau.dk/da/projects/lav-induktiv-igbt-sikring-low-inductance-igbt-fuse)
- [S] Located but not read: Dynex application note AN6442 on IGBT failure mechanisms — [Dynex AN6442](https://dynexsemi.com/Portals/0/PDF/DNX_AN6442.pdf); Renesas IGBT module application note — [Renesas](https://www.renesas.cn/zh/document/apn/igbt-module-application-note)

### Inferences
- The rupture literature confirms the project's "pop" anatomy in outline: the device fails short first, then the bus capacitor empties through it. The energy available in the team's design is 13.1 J at 373 V versus 1.9 J at 141 V, which matches the field observation that loud ruptures cluster at high line.
- The classical countermeasure is a fuse between the bulk capacitor and the bridge with a let-through I²t below the package rupture I²t. An AC-side fuse cannot do this, because the capacitor discharge does not pass through it. Whether a DC-link fuse is practical at this power level — and what the rupture I²t of a 1.5 A-class SOP/DIP half-bridge IPM is — was not found; the only numbers retrieved are for 400 A modules.
- Other levers that follow from the same energy argument: less bulk capacitance, a lower bus voltage, or a series impedance in the discharge path. Each trades against ripple or efficiency; none was found documented for dryers.
- Dyson's move from GaN (2024) back to silicon MOSFET modules (2026) suggests GaN did not win on cost at this power level; the teardowns give no stated reason.

### Gaps
- **Not found**: short-circuit withstand time of small MOSFET or IGBT IPMs and its dependence on junction temperature; datasheet comparison of integrated protection (over-current trip input, temperature output, interlock, under-voltage lockout, Miller clamp) across Infineon, onsemi, ST, Mitsubishi or the Chinese half-bridge IPMs used in dryers. No query on these ran.
- **Not found**: documented rupture cases or rupture I²t for SOP/DIP-packaged IPMs; any vendor guidance on preventing package rupture in sub-200 W drives.
- **Not found**: a cost or reliability comparison of IPM versus discrete versus GaN for 100–150 W; TI, Navitas, Infineon or Innoscience GaN motor-drive references for hair dryers.
- The project's statements on temperature-dependent short-circuit withstand and latch-up ([Appendix H §H.7](../../docs/21-附录H-极端高压角点风险清单.md)) could not be checked against vendor literature.

---

## 8. Applicable safety and EMC standards for hair dryers (IEC 60335-1, IEC 60335-2-23, IEC 60730 Class B, IEC 61000-3-2, CISPR 14)

### Takeaway
IEC 60335-2-23 was reissued as Edition 7.0 in 2026 with abnormal operation still in clause 19, and IEC 61000-3-2 explicitly names hair dryers among short-use portable appliances allowed asymmetrical control; clause-level test details, the software-class requirements and CISPR 14 limits were not retrieved.

### Cited Findings
- [S] IEC 60335-2-23 (appliances for skin or hair care) Edition 7.0 was issued in 2026; clause 19 is "Abnormal operation" — [iTeh sample of IEC 60335-2-23:2026](https://cdn.standards.iteh.ai/samples/iec/iec-60335-2-23-2026/6af181a5c6254c27a2b22c3c088d2151/iec-60335-2-23-2026.pdf); [VDE preview Ed.7.0](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec60335-2-23{ed7.0.CMV}en.pdf)
- [S] The previous consolidated edition is 6.1 (2019-03), also with abnormal operation in clause 19 — [VDE preview Ed.6.1](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec60335-2-23{ed6.1}b.pdf)
- [S] The Canadian adoption (CSA C22.2 No. 60335-2-23:21) added a requirement for protection against hair being drawn into hair-dryer air intakes — [Intertek notice](https://cdn.intertek.com/www-intertek-com/dms-legacy/CSA-E335-2-23-and-CSA-E60335-2-23-Superseded-by-CSA-C22-2-No-60335-2-23-ED-9-1-2025-Addendum-2.pdf)
- [S] IEC 61000-3-2 covers equipment with rated current up to 16 A; Edition 5 was published in 2018 and a consolidated Edition 5.1 preview is also listed — [Wikipedia](https://en.wikipedia.org/wiki/IEC_61000-3-2); [VDE preview Ed.5.0](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec61000-3-2{ed5.0}b.pdf); [VDE preview Ed.5.1](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec61000-3-2{ed5.1}b.pdf)
- [S] Hair dryers are listed among portable short-use equipment for which asymmetrical control is permitted; where the exemption applies, half-wave rectification may be used for any purpose while asymmetrical control is limited to motor control (2014 text from a third-party reproduction; not confirmed for the 2018 edition) — [CSDN文库 copy](https://wenku.csdn.net/doc/6m25o6bj48)

### Inferences
- A 2026 edition of the Part 2 standard means certification bodies will transition test programmes; the team should confirm which edition its current certificates reference and whether Edition 7.0 changes abnormal-operation or electronic-circuit requirements that bear on the motor-drive protection scheme.
- The half-wave exemption explains why dryer heaters can use diode half-power and TRIAC schemes; it says nothing about the motor-drive rectifier's harmonic class, which still has to be determined.

### Gaps
- **Not found**: the content of clause 19 in IEC 60335-2-23 (restricted inlet, locked rotor, thermal cut-out operation) and the changes made in Edition 7.0.
- **Not found**: IEC 60335-1 electronic-circuit fault and EMC-under-fault requirements, Annex R software evaluation, and how IEC 60730 Class B applies to a dryer whose over-current and over-temperature protection is firmware-based.
- **Not found**: the IEC 61000-3-2 equipment class and limits for the motor-drive front end; CISPR 14-1/-2 limits and current edition; Chinese equivalents (GB 4706.1, GB 4706.15, GB 4343.1, GB 17625.1) and their current status.
- **Not found**: electric-strength test voltages for basic and reinforced insulation at this working voltage, which bear on the motor hi-pot level discussed in section 5.
