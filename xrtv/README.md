# XRTV cargame backend (branch xrtv/cargame)

UEVR nightly 01143 (`4ee5c6b`) + the six XRTV patches that make the cargame UE 5.8 **Development** cook work in
extreme-compatibility mode + AFR over CloudXR (title-container `titles/cargame`, image `cargame:v1.0.9-release`):

1. XC: no Slate hook (`FFakeStereoRenderingHook`), the UE 5.8 Slate path crashed `ArrayView.h:396`
2. submit-state fallback (`runtimes/OpenXR.cpp`)
3. present-driven `update_hmd_state` in XC (UE 5.8 never calls xrLocateViews otherwise -> CloudXR never encodes)
4. AFR eye from `UGameEngine::Tick` (UE 5.8 has no `UGameViewportClient::Draw` hook)
5. present-parity AFR eye + phase vote (`XRTV_UEVR_SWAP_EYES`, `_EYE_POLARITY`, `_P3_SLOT`)
6. image-based AFR eye (`D3D11Component`, horizontal shift between consecutive presents)

Plus `XrtvTrace.hpp` (`XRTV_UEVR_EYE_TRACE=1`, off by default).

The UESDK submodule needs `xrtv/uesdk-ue58-uobjectarray.patch` (UE 5.8 GUObjectArray layout + GMalloc via
`XRTV_GUOBJECTARRAY_*` / `XRTV_GMALLOC_*` env overrides, values from the game PDB; inert when unset):

```powershell
git -c protocol.file.allow=always submodule update --init --recursive
git -C dependencies/submodules/UESDK apply --ignore-whitespace ..\..\..\xrtv\uesdk-ue58-uobjectarray.patch
Remove-Item Env:\NoDefaultCurrentDirectoryInExePath   # else DirectXTK's CompileShaders.cmd "is not recognized"
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target uevr --parallel
```

Rebuilt 2026-10-03 (md5 `7b757fee…`; the shipped `0303272b…` differs only in embedded build metadata): all 43
XRTV marker strings identical; headless on the A40 with `cargame:v1.0.9-release` -> session FOCUSED, overrides and
AFR eye active, 1 pose miss in 150 s. Unrelated to the borderlands3 branch `xrtv/present-guard` (stale-pose).
