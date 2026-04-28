"""CLI entry point for diode.

Subcommands:
  diode cmake-dir   Print the path to diode's bundled CMake modules so
                    consumers can do:

                      execute_process(COMMAND diode cmake-dir
                                      OUTPUT_VARIABLE DIODE_CMAKE_DIR
                                      OUTPUT_STRIP_TRAILING_WHITESPACE)
                      find_package(Diode REQUIRED CONFIG
                                   HINTS "${DIODE_CMAKE_DIR}")
"""

import argparse
import sys
from importlib.resources import files
from pathlib import Path


def _cmake_dir() -> str:
    pkg = files("diode").joinpath("cmake")
    if Path(str(pkg)).is_dir():
        return str(pkg)
    # Editable install: cmake/ lives at repo root, not inside the package.
    repo = Path(__file__).resolve().parents[3]
    fallback = repo / "cmake" / "modules"
    if fallback.is_dir():
        return str(fallback)
    raise FileNotFoundError("Cannot locate diode cmake modules")


def _cmd_cmake_dir(_args: argparse.Namespace) -> None:
    print(_cmake_dir())


def main() -> None:
    parser = argparse.ArgumentParser(
        prog="diode",
        description="diode — deterministic I/O test framework",
    )
    subparsers = parser.add_subparsers()

    p = subparsers.add_parser("cmake-dir", help="print cmake module path")
    p.set_defaults(func=_cmd_cmake_dir)

    args = parser.parse_args()
    if hasattr(args, "func"):
        args.func(args)
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    main()
