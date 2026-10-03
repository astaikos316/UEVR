# ABANDONED EXPERIMENT (branch xrtv/bl3-ahud-experiment) — do not ship

= branch `xrtv/cargame` (six cargame patches + UESDK patch) PLUS the Borderlands 3 AHUD bisect switches
`XRTV_UEVR_AHUD_NO_PASSTHROUGH` / `XRTV_UEVR_AHUD_FORCE_REDIRECT` / `XRTV_UEVR_AHUD_FORCE_ORIGINAL`
(comma-separated exe RVAs) in `FFakeStereoRenderingHook.cpp`, built once as `xrtv-ahud` (md5 447abef5…).

Purpose (2026-10-02): bisect which BL3 callers carried the Scaleform menu when menus landed in one eye. Superseded
by `VR_NativeStereoFix=false` (borderlands3 DELTA #N), which fixed it without any UEVR change. Kept only for history.
Shipping branches: `xrtv/present-guard` (borderlands3), `xrtv/cargame` (cargame / UE5 Dev / AFR).
