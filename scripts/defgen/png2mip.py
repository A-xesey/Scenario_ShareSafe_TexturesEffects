#!/usr/bin/env python3

'''Compiles a M-i-P with the custom image, mask, and data.'''




from io import BytesIO
from typing import Callable, BinaryIO
import zopfli.png
from PIL import Image, ImageFile
from spore_ep1stegano import Stegano, CRC32_BGRA32
from .mip_metadata import MiPHeader




BGRA = tuple[bytes, tuple[int, int]]
'''Represents the image in BGRA with its dimensions (pixels, (width, height)).'''

def _resize(image: Image.Image, dimensions: tuple[int, int]) -> Image.Image:
    '''Resizes the image file with bicubic resampling.

    Arguments:
        image (Image.Image): The image to resize.
        dimensions (tuple[int, int]): The target pair of dimensions (width, height).

    Returns:
        out (Image.Image): The output image RGBA pixels and its dimensions.
    '''
    return image.resize(dimensions, Image.Resampling.BICUBIC)

def _read_image(image: bytes | ImageFile.ImageFile | BinaryIO,
                convert: Callable[[Image.Image], Image.Image] = lambda image: image) -> Image.Image:
    '''Reads the image and applies conversion.

    Attributes:
        image (bytes | ImageFile.ImageFile | BinaryIO): The image to read.
        convert (Callable[[Image.Image], Image.Image]): The additional conversion to apply before
                                                        returning the image.
    
    Returns:
        out (Image.Image): The output image.
    '''
    if isinstance(image, ImageFile.ImageFile):
        return convert(image.convert('RGBA'))
    if isinstance(image, bytes):
        image = BytesIO(image)
    image_file: ImageFile.ImageFile
    with Image.open(image) as image_file:
        return convert(image_file.convert('RGBA'))




def read_image(image: bytes | ImageFile.ImageFile | BinaryIO,
               mask: bytes | ImageFile.ImageFile | BinaryIO | None = None) -> BGRA:
    '''Reads the image and applies the mask PNG to it.

    Arguments:
        image (bytes | ImageFile.ImageFile | BinaryIO): The image to apply the mask to.
        mask (bytes | ImageFile.ImageFile | BinaryIO | None): The mask to apply.
    
    Returns:
        out (BGRA): The output image BGRA pixels and its dimensions.
    '''
    r: Image.Image
    g: Image.Image
    b: Image.Image
    a: Image.Image
    image_rgba: Image.Image
    if mask:
        mask_rgba: Image.Image = _read_image(mask)
        image_rgba = _read_image(image, lambda image: _resize(image, mask_rgba.size))
        r, g, b, _ = image_rgba.split()
        a, _, _, _ = mask_rgba.split()
    else:
        image_rgba = _read_image(image)
        r, g, b, a = image_rgba.split()
    return (Image.merge('RGBA', (b, g, r, a)).tobytes(), image_rgba.size)

def stegano_from(image: BGRA,
                 header: MiPHeader,
                 data: bytes | bytearray,
                 use_zopfli: bool = True) -> Stegano:
    '''Forms a stegano from the given header and the image data (used to generate the checksum).

    Arguments:
        header (MiPHeader): The Model-in-Picture header with all the metadata fields filled.
        image (BGRA): The image data to generate a checksum from. This specific stegano shouldn't
                      be added to any other image.
        data (bytes | bytearray): The data to use as the contents of a file.
        use_zopfli (bool): Determines if Zopfli should be used to improve compression.
                           Default: True.
    
    Returns:
        out (Stegano): The stegano data.
    '''
    image_pixels: bytes
    image_pixels, _ = image
    return Stegano(CRC32_BGRA32(image_pixels), bytes(header) + bytes(data), use_zopfli)




ZOPFLIPNG_KWARGS: dict = {
    'lossy_transparent': False,
    'lossy_8bit': False
}

def bgra_to_png(image_bgra: BGRA, use_zopfli: bool = True) -> bytes:
    ''''Packs the BGRA image to the PNG file.
    
    Attributes:
        image (BGRA): The image data.
        use_zopfli (bool): Determines if ZopfliPNG should be used to improve compression.
                           Default: True.
    
    Returns:
        out (bytes): The bytes of a compressed PNG image.
    '''
    image_pixels: bytes
    image_dimensions: tuple[int, int]
    image_pixels, image_dimensions = image_bgra
    image: Image.Image = Image.frombytes('RGBA', image_dimensions, image_pixels, 'raw', 'BGRA')

    png: BytesIO
    with BytesIO() as png:
        image.save(png, format='PNG', compress_level=9)
        return (png.getvalue()
                if not use_zopfli
                else zopfli.png.optimize(png.getvalue(), **ZOPFLIPNG_KWARGS))

def bgra_to_mip(image_bgra: BGRA,
                header: MiPHeader,
                data: bytes | bytearray,
                use_zopfli: bool = True) -> bytes:
    '''Adds the stegano data to the image and packs it to the PNG file.
    
    Arguments:
        header (MiPHeader): The Model-in-Picture header with all the metadata fields filled.
        image (BGRA): The image data.
        data (bytes | bytearray): The data to use as the contents of a file.
        use_zopfli (bool): Determines if Zopfli & ZopfliPNG should be used to improve compression.
                           Default: True.
    
    Returns:
        out (Stegano): The stegano data.
    '''
    stegano: Stegano = stegano_from(image_bgra, header, data, use_zopfli)
    return bgra_to_png(image_bgra, use_zopfli) + stegano.compile()
