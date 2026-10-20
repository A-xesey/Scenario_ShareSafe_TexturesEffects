#!/usr/bin/env python3

'''Generates Model-in-Picture (M-i-P) v6 metadata headers.'''




from typing import Final
from fixedint import UInt32, UInt64
from spore_globals.resource_key import ResourceKey
from spore_globals.string import String8, String16
from spore_globals.timestamp import Timestamp




ServerID = UInt64
'''The typical server ID, Spore.com normally uses 500*/501* IDs.
More details on Spore server IDs: https://wiki.archiveteam.org/index.php/Spore#Numeric_IDs

Example:
    500123456789
'''

ServerIDLineage = tuple[ServerID, ServerID]
'''Two server IDs for an asset: the first value is the ID of this exact asset, the second is the ID
of the "original" creation it's based on.
'''

TraitID = UInt32
'''The consequence trait ID.'''




class MiPHeader:
    '''A class that represents Spore Model-in-Picture (M-i-P) stegano metadata header
    in a modifiable form.

    Attributes:
        resource_key (ResourceKey): The resource key of the encoded file.
        timestamp (Timestamp): The timestamp of when the asset was saved. Generates
                               the current timestamp if not specified.
        machine_id (UInt32): The unique device fingerprint that is normally calculated from
                             the OS version, the CPU model, and the serial number of the HDD with
                             the game installed. Default: 0x00000000.
        asset_ids (ServerIDLineage): Two server IDs for an asset: the first value is the ID of this
                                     exact asset, the second is the ID of the "original" creation
                                     it's based on.
        asset_name (String16): The name of the asset (max. 255 bytes).
        asset_desc (String16): The description of the asset (max. 4095 bytes).
        asset_tags (String16): The tags of the asset (max. 255 bytes).
        asset_traits (list[TraitID]): The list of consequence traits (max. 255 traits).
        user_id (ServerID): The asset creator server ID.
        user_name (String16): The name of the asset creator (max. 255 bytes).
    '''
    VERSION: Final[int] = 6 # 0000-9999
    '''The supported version. Known versions are:
    * 3 (Feb2008: compiled creations, likely January 2008)
    * 4 (Feb2008: saved creations)
    * 5 (CSA Trial/unpatched)
    * 6 (Spore, Spore: GA)
    * 7 (Darkspore)
    '''

    machine_id: UInt32
    '''The unique device fingerprint that is normally calculated from the OS version, the CPU model,
    and the serial number of the HDD with the game installed.
    '''
    asset_ids: ServerIDLineage
    '''Two server IDs for an asset: the first value is the ID of this exact asset, the second is
    the ID of the "original" creation it's based on.
    '''
    user_id: ServerID
    '''The asset creator server ID.'''

    def __init__(self,
                 resource_key: ResourceKey | None = None,
                 timestamp: Timestamp | None = None,
                 machine_id: UInt32 = UInt32(),
                 asset_ids: ServerIDLineage = (ServerID(-1), ServerID(-1)),
                 asset_name: String8 | String16 | str | None = None,
                 asset_desc: String8 | String16 | str | None = None,
                 asset_tags: String8 | String16 | str | None = None,
                 asset_traits: list[TraitID] | None = None,
                 user_id: ServerID = ServerID(-2),
                 user_name: String8 | String16 | str | None = None) -> None:
        self.resource_key = resource_key
        self.timestamp = timestamp
        self.machine_id = machine_id
        self.asset_ids = asset_ids
        self.asset_name = asset_name
        self.asset_desc = asset_desc
        self.asset_tags = asset_tags
        self.asset_traits = asset_traits
        self.user_id = user_id
        self.user_name = user_name

    @staticmethod
    def _get_string_size(string: String8 | String16 | str) -> int:
        return len(string.encode('utf-8'))

    @property
    def resource_key(self) -> ResourceKey:
        '''The resource key of the encoded file.'''
        return self._resource_key
    @resource_key.setter
    def resource_key(self, resource_key_modified: ResourceKey | None) -> None:
        self._resource_key: ResourceKey = (resource_key_modified
                                           if resource_key_modified is not None else ResourceKey())

    @property
    def timestamp(self) -> Timestamp:
        '''The timestamp of when the asset was saved.'''
        return self._timestamp
    @timestamp.setter
    def timestamp(self, ts_modified: Timestamp | None) -> None:
        self._timestamp: Timestamp = ts_modified if ts_modified is not None else Timestamp()

    @property
    def asset_name(self) -> String16:
        '''The name of the asset (max. 255 bytes).'''
        return self._asset_name
    @asset_name.setter
    def asset_name(self, asset_name_modified: String8 | String16 | str | None) -> None:
        if asset_name_modified is None:
            self._asset_name: String16 = String16()
        else:
            if MiPHeader._get_string_size(asset_name_modified) > 0xff:
                raise ValueError('The asset_name string is longer 255 (0xff) bytes.')
            self._asset_name = String16(str(asset_name_modified))

    @property
    def asset_desc(self) -> String16:
        '''The description of the asset (max. 4095 bytes).'''
        return self._asset_desc
    @asset_desc.setter
    def asset_desc(self, asset_desc_modified: String8 | String16 | str | None) -> None:
        if asset_desc_modified is None:
            self._asset_desc: String16 = String16()
        else:
            if MiPHeader._get_string_size(asset_desc_modified) > 0xfff:
                raise ValueError('The asset_desc string is longer 4095 (0xfff) bytes.')
            self._asset_desc = String16(str(asset_desc_modified))

    @property
    def asset_tags(self) -> String16:
        '''The tags of the asset (max. 255 bytes).'''
        return self._asset_tags
    @asset_tags.setter
    def asset_tags(self, asset_tags_modified: String8 | String16 | str | None) -> None:
        if asset_tags_modified is None:
            self._asset_tags: String16 = String16()
        else:
            if MiPHeader._get_string_size(asset_tags_modified) > 0xff:
                raise ValueError('The asset_tags string is longer 255 (0xff) bytes.')
            self._asset_tags = String16(str(asset_tags_modified))

    @property
    def asset_traits(self) -> list[TraitID]:
        '''The list of consequence traits (max. 255 traits).'''
        return self._asset_traits
    @asset_traits.setter
    def asset_traits(self, asset_traits_modified: list[TraitID] | None) -> None:
        if asset_traits_modified is None:
            self._asset_traits: list[TraitID] = []
        else:
            if len(asset_traits_modified) > 0xff:
                raise ValueError('The asset_traits list has more than 255 (0xff) trait IDs.')
            self._asset_traits = asset_traits_modified

    @property
    def user_name(self) -> String16:
        '''The name of the asset creator (max. 255 bytes).'''
        return self._user_name
    @user_name.setter
    def user_name(self, user_name_modified: String8 | String16 | str | None) -> None:
        if user_name_modified is None:
            self._user_name: String16 = String16()
        else:
            if MiPHeader._get_string_size(user_name_modified) > 0xff:
                raise ValueError('The user_name string is longer 255 (0xff) bytes.')
            self._user_name = String16(str(user_name_modified))

    def __str__(self) -> str:
        '''Gets a stringified M-i-P header.
        
        Returns:
            out (str): The M-i-P header string with all metadata fields included.
        
        Raises:
            ValueError: if asset_traits contains more than 255 trait IDs.
        '''
        self.asset_traits = self._asset_traits
        return ('spore'
                + f'{MiPHeader.VERSION:04d}'
                + f'{self.resource_key.type_id:08x}'
                + f'{self.resource_key.group_id:08x}'
                + f'{self.resource_key.instance_id:08x}'
                + f'{self.machine_id:08x}'
                + f'{self.asset_ids[0]:016x}{self.asset_ids[1]:016x}'
                + f'{self.timestamp.year1():016x}'
                + f'{MiPHeader._get_string_size(self.user_name):02x}{self.user_name}'
                + f'{self.user_id:016x}'
                + f'{MiPHeader._get_string_size(self.asset_name):02x}{self.asset_name}'
                + f'{MiPHeader._get_string_size(self.asset_desc):03x}{self.asset_desc}'
                + f'{MiPHeader._get_string_size(self.asset_tags):02x}{self.asset_tags}'
                + f'{len(self.asset_traits):02x}'
                + ''.join(f'{trait_id:08x}' for trait_id in sorted(self.asset_traits)))

    def __bytes__(self) -> bytes:
        '''Gets a stringified M-i-P header, encoded with UTF-8.
        
        Returns:
            out (bytes): The M-i-P header with all metadata fields included.
        
        Raises:
            ValueError: if asset_traits contains more than 255 trait IDs.
        '''
        return str(self).encode()
