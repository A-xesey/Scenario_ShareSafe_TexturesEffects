#!/usr/bin/env python3

'''Generates the definition for a ground texture or a visual effect from the CLI.'''#TODO:docstrings




import argparse
import logging
import sys
from contextlib import ExitStack
from io import BytesIO, BufferedReader, BufferedWriter
from pathlib import Path
from PIL import UnidentifiedImageError
from spore_globals.fnv import fnv1
from spore_globals.property import *
from .mip_metadata import MiPHeader, ServerID
from .png2mip import BGRA, read_image, bgra_to_mip




# type ids
PROP: UInt32 = UInt32(0xb1b104)

# group ids
SCENARIO_TEXTURE: UInt32 = UInt32(0x408a2100)
SCENARIO_FILTER: UInt32 = UInt32(0x408a6f00)

# property ids
PARENT: UInt32 = UInt32(0xb2cccb)
CUSTOMIZATION_ITEM_KEY: UInt32 = UInt32(fnv1('CustomizationItemKey'))
CUSTOMIZATION_ITEM_NAME: UInt32 = UInt32(fnv1('CustomizationItemName'))




_DEFAULT_TEMPLATE_TEXTURE: ResourceKey | None = ResourceKey(fnv1('TemplateTextureCustom'),
                                                            fnv1('ScenarioCustomizationTemplates'))
_DEFAULT_TEMPLATE_EFFECT: ResourceKey | None = None

_DEFAULT_METADATA: dict = {
    'machine_id': UInt32(0xc7659eb1),
    'asset_name': 'SSSTE Definition',
    'user_name': 'defgen',
    'user_id': ServerID(501125112375)
}




def _parse_uint32(string: str | None) -> UInt32 | None:
    if not string:
        return None
    return UInt32(int(string, 16)) if string.startswith('0x') else fnv1(string)

def _parse_key(key: str) -> ResourceKey:
    instance_id: str
    type_id: str | None = None
    group_id: str | None = None
    group_id, _, instance_id = key.rpartition('!')
    instance_id, _, type_id = instance_id.partition('.')
    return ResourceKey(_parse_uint32(instance_id),
                       _parse_uint32(type_id),
                       _parse_uint32(group_id))

def _action_encode(customization_key: ResourceKey,
                   definition_key: ResourceKey,
                   image_path: str,
                   output_path: str,
                   mask_path: str | None = None,
                   template_key: ResourceKey | None = None,
                   name: str | None = None,
                   use_zopfli: bool = True) -> int:
    logging.info(f'Generating \"{output_path}\" with thumbnail \"{image_path}\"'
                 + f' and mask \"{mask_path}\"' if mask_path else ''
                 + '...')
    if name:
        logging.info('%s:', name)
    else:
        name = str(customization_key)
    logging.info('The customization key is %s. The definition key is %s.',
                 customization_key,
                 definition_key)
    if template_key:
        logging.info('(the template is %s)', customization_key)
    logging.info('Zopfli & ZopfliPNG are ' + 'ON' if use_zopfli else 'OFF' + '.')

    stack: ExitStack
    with ExitStack() as stack:
        image_file: BufferedReader
        try:
            image_file = stack.enter_context(open(image_path, mode='rb'))
        except OSError:
            logging.exception('Error on opening the thumbnail (%s)', image_path)
            return 1

        mask_file: BufferedReader | None = None
        if mask_path:
            try:
                mask_file = stack.enter_context(open(mask_path, mode='rb'))
            except OSError:
                logging.exception('Error on opening the mask (%s)', mask_path)
                return 1

        image: BGRA
        logging.info('Forming the BGRA image data...')
        try:
            image = read_image(image_file, mask_file)
        except UnidentifiedImageError:
            logging.exception('Unidentified image format')
            return 1

        logging.info('Creating the Model-in-Picture header...')
        header: MiPHeader = MiPHeader(definition_key, **_DEFAULT_METADATA)
        logging.info('M-i-P header: %s', header)

        logging.info('Creating the property list...')
        proplist: PropertyList = {
            CUSTOMIZATION_ITEM_KEY: customization_key,
            CUSTOMIZATION_ITEM_NAME: [Text(placeholder=Text.String16Placeholder(name))]
        }
        if template_key:
            proplist |= {
                PARENT: template_key
            }

        logging.info('Serializing the property list...')
        proplist_io: BytesIO
        with BytesIO() as proplist_io:
            serialize(proplist, proplist_io)
            proplist_bytes: bytes = proplist_io.getvalue()
        logging.info('Property list size: %d bytes, with %d properties.',
                     len(proplist_bytes),
                     len(proplist))

        logging.info('Encoding the result PNG...')
        png: bytes = bgra_to_mip(image, header, proplist_bytes, use_zopfli)

        logging.info('Writing to %s...', output_path)
        output_file: BufferedWriter
        try:
            output_file = stack.enter_context(open(output_path, mode='wb'))
            output_file.write(png)
        except OSError:
            logging.exception('Error on writing to the output file (%s)', output_path)
            return 1

        logging.info('Done. Have a good Timestamp().')
        return 0

def _action_main() -> int:
    status_code: int = 1
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        prog='defgen',
        description=('(Def)inition (gen)erator for ground textures & effects that packs those into'
                     + 'Spore M-i-Ps.')
    )
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Log everything to the console'
    )
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    # textures
    parser_texture = subparsers.add_parser('texture',
                                           help='Create a definition for a ground texture',
                                           aliases=['tex'])
    parser_texture.add_argument('input_key',
                                type=str,
                                help='Input texture resource key')
    parser_texture.add_argument('input_thumb',
                                type=str,
                                help='Input texture thumbnail')
    parser_texture.add_argument('-m', '--input_mask',
                                type=str,
                                help='Input texture thumbnail alpha mask',
                                required=False)
    parser_texture.add_argument('-t', '--template',
                                type=str,
                                help='Definition template resource key',
                                required=False)
    parser_texture.add_argument('-n', '--name',
                                type=str,
                                help='Texture name',
                                required=False)
    parser_texture.add_argument('-o', '--output_png_path',
                                type=str,
                                help='Output PNG image',
                                required=False)
    parser_texture.add_argument('--no_zopfli',
                                help='Use the default Zlib library for compression',
                                required=False,
                                dest='use_zopfli',
                                action='store_false')
    parser_texture.set_defaults(use_zopfli=True)
    # effects
    parser_effect = subparsers.add_parser('effect',
                                          help='Create a definition for a visual effect',
                                          aliases=['fx'])
    parser_effect.add_argument('input_instance_id',
                               type=str,
                               help='Input effect instance ID')
    parser_effect.add_argument('input_thumb',
                               type=str,
                               help='Input effect thumbnail')
    parser_effect.add_argument('-m', '--input_mask',
                               type=str,
                               help='Input effect thumbnail alpha mask',
                               required=False)
    parser_effect.add_argument('-t', '--template',
                               type=str,
                               help='Definition template resource key',
                               required=False)
    parser_effect.add_argument('-n', '--name',
                               type=str,
                               help='Effect name',
                               required=False)
    parser_effect.add_argument('-o', '--output_png_path',
                               type=str,
                               help='Output PNG image',
                               required=False)
    parser_effect.add_argument('--no_zopfli',
                               help='Use the default Zlib library for compression',
                               required=False,
                               dest='use_zopfli',
                               action='store_false')
    parser_effect.set_defaults(use_zopfli=True)

    args: argparse.Namespace = parser.parse_args()
    if not args.command:
        parser.print_help()
        return status_code
    logging.basicConfig(level=logging.INFO if args.verbose else logging.WARNING,
                        format='%(message)s')

    if not args.output_png_path:
        output_file_name: str = (args.input_key
                                 if args.command in ['texture', 'tex']
                                 else args.input_instance_id) + '.png'
        args.output_png_path = str(Path.cwd() / output_file_name)
    # textures: action
    if args.command in ['texture', 'tex']:
        return _action_encode(_parse_key(args.input_key),
                              ResourceKey(type_id=PROP, group_id=SCENARIO_TEXTURE),
                              args.input_thumb,
                              args.output_png_path,
                              args.input_mask,
                              args.template if args.template else _DEFAULT_TEMPLATE_TEXTURE,
                              args.name,
                              args.use_zopfli)
    # effects: action
    elif args.command in ['effect', 'fx']:
        return _action_encode(ResourceKey(_parse_uint32(args.input_instance_id)),
                              ResourceKey(type_id=PROP, group_id=SCENARIO_FILTER),
                              args.input_thumb,
                              args.output_png_path,
                              args.input_mask,
                              args.template if args.template else _DEFAULT_TEMPLATE_EFFECT,
                              args.name,
                              args.use_zopfli)
    return status_code

if __name__ == '__main__':
    sys.exit(_action_main())
