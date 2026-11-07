# Removed code, for reference

Code deleted from this repo that we may want to bring back. Restore a file or
directory from the last commit that still had it, for example:

    git show ad50a10:functions/generateFeature/p_pointcloudsegmentation.h
    git restore --source=ad50a10 -- functions/generateFeature

## Point cloud segmentation (`functions/generateFeature/`)

Removed in `2791a09` (remove dead point cloud segmentation sources). The
directory (23 files: `p_pointcloudsegmentation.*` and
`pointcloud_segmentation/ps_*`: octree, node, plane/cylinder/sphere segments,
point cloud, general math) was not part of the CMake build, and it used the
Qt5-era `PS_LoadingDialog`, which `p_pointcloudsegmentation.h` embedded and
showed while the function ran.

| Commit | State of the code |
|---|---|
| `ad50a10` | Last commit that has it (parent of the removal). Already uses `std::string`, `std::vector` and `oi::Guarded`. Still has `PS_LoadingDialog`. |
| `dc1a335` | Same, before the `std::string` change: still `QString`/`QList` at the core boundary. |
| `dc123b4` | The original import, before the core rework. |

To revive it: report progress through the `OperationContext` that
`Function::operation()` provides instead of `PS_LoadingDialog`, add the files
to `CMakeLists.txt`, and register the function in `p_factory.cpp`.

## Other code that is still here but not built

`exchange/p_oiexchangepts.*`, `p_extendedtemperaturecompensation` and
`p_pseudosensor` are in the tree but built nowhere.
