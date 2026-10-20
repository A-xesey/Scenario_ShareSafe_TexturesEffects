#!/usr/bin/env python3

'''(Def)inition (gen)erator for ground textures & effects that packs those into Spore M-i-Ps.'''

__author__ = '0KepOnline' # https://github.com/0KepOnline
__license__ = 'MIT' # see: "../../LICENSE"
__maintainer__ = '0KepOnline'
__email__ = 'i.am@scenariopla.net'




import sys
from pathlib import Path




# these aren't installed with pip and use git submodules instead
_PREREQUISITES: list[str] = [
    'SporeGlobals-Python',
    'Spore_EP1Stegano-Python'
]

for parent in Path(__file__).resolve().parents:
    prerequisites: Path = parent / 'prerequisites'
    if prerequisites.is_dir():
        for prerequisite_name in _PREREQUISITES:
            path: str = str(prerequisites / prerequisite_name)
            if path not in sys.path:
                sys.path.append(path)
        break
