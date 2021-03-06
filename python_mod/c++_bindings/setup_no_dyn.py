# Creates a relative path to this source file location on the filesystem, and then removes the last 64 characters of the str
# that are the ones after the root of the project's path ( '\\rumble_league_extension_plugin' )
from pathlib import Path
rel_path = str(Path(__file__).absolute())

# import os
# os.chdir(rel_path)

import subprocess
import sys

from setuptools import setup, Extension

try:
    import pybind11
except ImportError:
    if subprocess.call([sys.executable, '-m', 'pip', 'install', 'pybind11']):
        raise RuntimeError('pybind11 install failed.')


# import pybind11

cpp_args = [
    "-IC:\\vcpkg\\installed\\x64-windows\\include",
    f"-I{rel_path}\\rumble_league_extension_plugin\X64\RELEASE",
    "/link",
    "/LIBPATH:C:\\vcpkg\\installed\\x64-windows\\lib",
    "/LIBPATH:C:\\vcpkg\\installed\\x64-windows\\bin",
    "C:\\vcpkg\\installed\\x64-windows\\lib\\opencv_core.lib",
    "C:\\vcpkg\\installed\\x64-windows\\lib\\opencv_highgui.lib",
    f"/LIBPATH:{rel_path}\\rumble_league_extension_plugin\X64\RELEASE",
    "/LIBPATH:C:\\vcpkg\\installed\\x64-windows\\lib\\*.lib",
    '/c "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\HostX86\x64\link.exe"',
 ]

sfc_module = Extension(
    'rle',
    sources=[
        # Python bindings
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\python_mod\c++_bindings\python_linker.cpp',
        # Main library
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\core\RumbleLeague.cpp',
        # League Client screens and buttons
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\core\league_client\LeagueClientScreen.cpp',
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\core\league_client\LeagueClientButton.cpp',
        # Motion
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\motion\RumbleMotion.cpp',
        # Vision
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\gision\RumbleVision.cpp',
        # Window Capture
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\window_capture\WindowCapture.cpp',
        # Writer
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\writer\RumbleWriter.cpp',
        # Helpers
        f'D:\MSi 2020-2021\Code\Python\Rumble-AI\src\plugins\\rumble_league_extension_plugin\helpers\StringHelper.cpp',
        
    ],
    include_dirs=[
        pybind11.get_include(),
    ],
    language='c++',
    extra_compile_args=cpp_args,
    )

setup(
    name='rumble_league_extension',
    version='1.0.0',
    description='Python package that extends the Rumble-AI project with this module for voice control and AI related mechanics',
    ext_modules=[sfc_module],
)