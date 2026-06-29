"""Executable entry point for the terminal prototype."""

if __package__ in {None, ""}:
    import os
    import sys

    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from prototype_python.ui_terminal import run_terminal
else:
    from .ui_terminal import run_terminal


if __name__ == "__main__":
    run_terminal()
