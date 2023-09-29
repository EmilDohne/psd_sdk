# PSB Support

## Read

- [x] File Header Version
- [x] Length of Layer and Mask Information Section (uint64_t instead of uint32_t)
- [x] Length of Layer Info Section (uint64_t instead of uint32_t)
- [x] Layer Record length (uint64_t instead of uint32_t)
- [x] RLE Count of Scanlines in channel (uint32_t instead of uint16_t)
- [x] Additional Layer information length for the following keys (uint64_t instead of uint32_t)
    ```
    LMsk, 
    Lr16, 
    Lr32, 
    Layr, 
    Mt16, 
    Mt32, 
    Mtrn, 
    Alph, 
    FMsk, 
    lnk2, 
    FEid, 
    FXid, 
    PxSD
    ```


--> Parsing a PSB with greater than 32,768 x 32,768 dimensions for 32-bit files to test the updated `SyncfileReader::Read()` method with a `uint64_t` count instead of previously `uint32_t`


## Write

- [ ] File Header Version
- [ ] Length of Layer and Mask Information Section (uint64_t instead of uint32_t)
- [ ] Length of Layer Info Section (uint64_t instead of uint32_t)
- [ ] Layer Record length (uint64_t instead of uint32_t)
- [ ] RLE Count of Scanlines in channel (uint32_t instead of uint16_t)
- [ ] Additional Layer information length for the following keys (uint64_t instead of uint32_t)
    ```
    LMsk, 
    Lr16, 
    Lr32, 
    Layr, 
    Mt16, 
    Mt32, 
    Mtrn, 
    Alph, 
    FMsk, 
    lnk2, 
    FEid, 
    FXid, 
    PxSD
    ```



# Nested structures support

Implement the Layer Group information (tag 1026). These values apparently do not need to adhere to any standard, just that each unique group has a unique index as uint16_t. It is unclear how nested layers get treated, if the group id propagates down or not.

The Layer Group Enabled ID (tag 1072) does not seem to be relevant anymore as can be seen by the [pytoshop](https://github.com/mdboom/pytoshop/) implementation not using it and nested layers work there. 

Update --> This does not seem to be the case anymore and the tag 1026 is not used when saving from modern PS versions. What seems to determine layers is the `lsct` Key in the additional layer information section.

PSD_SDK does already provide functionality for parsing this data when reading but not for write operations. Bonus: add examples of how to parse layered data in PSDSamples.cpp for read operations

### Implementation ideas

Specialize layers such that there is a `Layer` class but also a `Group` class to abstract away implementation details such as opening and closing a layer but rather having PSD_SDK handle this automatically