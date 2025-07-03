/**
 * @file    GSCA/AudioStore.c
 */

#include <GSCA/AudioStore.h>

/* Private Constants **********************************************************/

#define GSCA_AS_HANDLES_INIT_CAPACITY   8
#define GSCA_AS_MAGIC_NUMBER            0x41435347

/* Static Members *************************************************************/

static uint16_t gscaNextId = 1;

/* Audio File Header Structure ************************************************/

typedef struct
{
    uint32_t    magicNumber;    ///< @brief `$0000` - 4 byte magic identifier `GSCA`.
    uint16_t    audioCount;     ///< @brief `$0006` - Number of audio entries expected.
    uint8_t     majorVersion;   ///< @brief `$0004` - Major version - must be `==` library version.
    uint8_t     minorVersion;   ///< @brief `$0005` - Minor version - must be `<=` library version.
} gscaAudioFileHeader;

/* Audio Store Structure ******************************************************/

typedef struct gscaAudioStore
{
    gscaAudioHandle*    handles;
    size_t              handlesSize;
    size_t              handlesCapacity;

    uint8_t*            data;
    size_t              dataSize;
    size_t              dataCapacity;
} gscaAudioStore;

/* Private Function Prototypes ************************************************/

static void gscaInitContainers (gscaAudioStore*, size_t);
static void gscaResizeHandlesArray (gscaAudioStore*);
static void gscaResizeDataBuffer (gscaAudioStore*, size_t);
static bool gscaReadByte (FILE*, uint8_t*);
static bool gscaReadWord (FILE*, uint16_t*);
static bool gscaReadDoubleWord (FILE*, uint32_t*);
static bool gscaReadQuadWord (FILE*, uint64_t*);
static bool gscaReadString (FILE*, char*, size_t);
static bool gscaWriteByte (FILE*, const uint8_t);
static bool gscaWriteWord (FILE*, const uint16_t);
static bool gscaWriteDoubleWord (FILE*, const uint32_t);
static bool gscaWriteQuadWord (FILE*, const uint64_t);
static bool gscaWriteString (FILE*, const char*, size_t);

/* Private Functions **********************************************************/

void gscaInitContainers (gscaAudioStore* audioStore, size_t initDataCapacity)
{
    // Initialize handles container.
    {
        audioStore->handles = gscaCreate(GSCA_AS_HANDLES_INIT_CAPACITY, gscaAudioHandle);
        gscaExpectp(audioStore->handles, "Could not allocate audio store handles array");
        audioStore->handlesSize = 0;
        audioStore->handlesCapacity = GSCA_AS_HANDLES_INIT_CAPACITY;
    }

    // Initialize data buffer.
    {
        audioStore->data = gscaCreate(initDataCapacity, uint8_t);
        gscaExpectp(audioStore->data, "Could not allocate audio store data buffer");
        audioStore->dataSize = 0;
        audioStore->dataCapacity = initDataCapacity;
    }
}

void gscaResizeHandlesArray (gscaAudioStore* audioStore)
{
    if (audioStore->handlesSize + 1 >= audioStore->handlesCapacity)
    {
        audioStore->handlesCapacity *= 2;
    }

    gscaAudioHandle* handles =
        gscaResize(audioStore->handles, audioStore->handlesCapacity, gscaAudioHandle);
    gscaExpectp(handles, "Could not resize audio store handles array");
    audioStore->handles = handles;
}

void gscaResizeDataBuffer (gscaAudioStore* audioStore, size_t extraSize)
{
    size_t dataCapacity = audioStore->dataCapacity;
    while (audioStore->dataSize + extraSize + 1 >= dataCapacity)
    {
        dataCapacity *= 1.75f;
    }

    if (dataCapacity > audioStore->dataCapacity)
    {
        uint8_t* data = gscaResize(audioStore->data, dataCapacity, uint8_t);
        gscaExpectp(data, "Could not resize audio store data buffer");
        audioStore->data = data;
        audioStore->dataCapacity = dataCapacity;
    }
}

bool gscaReadByte (FILE* fp, uint8_t* value)
{
    fread(value, sizeof(uint8_t), 1, fp);
    if (feof(fp))
    {
        gscaErr("Unexpected end-of-file reached while reading byte.\n");
        return false;
    }
    else if (ferror(fp))
    {
        gscaErrp("Read error occured while reading byte");
        return false;
    }

    return true;
}

bool gscaReadWord (FILE* fp, uint16_t* value)
{
    static uint8_t readWordBytes[2] = { 0 };
    fread(&readWordBytes, sizeof(uint8_t), 2, fp);
    if (feof(fp))
    {
        gscaErr("Unexpected end-of-file reached while reading word.\n");
        return false;
    }
    else if (ferror(fp))
    {
        gscaErrp("Read error occured while reading word");
        return false;
    }

    *value =
        (((uint16_t) readWordBytes[0] <<  0)) |
        (((uint16_t) readWordBytes[1] <<  8));

    return true;
}

bool gscaReadDoubleWord (FILE* fp, uint32_t* value)
{
    static uint8_t readDoubleWordBytes[4] = { 0 };
    fread(&readDoubleWordBytes, sizeof(uint8_t), 4, fp);
    if (feof(fp))
    {
        gscaErr("Unexpected end-of-file reached while reading double word.\n");
        return false;
    }
    else if (ferror(fp))
    {
        gscaErrp("Read error occured while reading double word");
        return false;
    }

    *value =
        (((uint32_t) readDoubleWordBytes[0] <<  0)) |
        (((uint32_t) readDoubleWordBytes[1] <<  8)) |
        (((uint32_t) readDoubleWordBytes[2] << 16)) |
        (((uint32_t) readDoubleWordBytes[3] << 24));

    return true;
}

bool gscaReadQuadWord (FILE* fp, uint64_t* value)
{
    static uint8_t readQuadWordBytes[8] = { 0 };
    fread(&readQuadWordBytes, sizeof(uint8_t), 8, fp);
    if (feof(fp))
    {
        gscaErr("Unexpected end-of-file reached while reading quad word.\n");
        return false;
    }
    else if (ferror(fp))
    {
        gscaErrp("Read error occured while reading quad word");
        return false;
    }

    *value =
        (((uint64_t) readQuadWordBytes[0] <<  0)) |
        (((uint64_t) readQuadWordBytes[1] <<  8)) |
        (((uint64_t) readQuadWordBytes[2] << 16)) |
        (((uint64_t) readQuadWordBytes[3] << 24)) |
        (((uint64_t) readQuadWordBytes[4] << 32)) |
        (((uint64_t) readQuadWordBytes[5] << 40)) |
        (((uint64_t) readQuadWordBytes[6] << 48)) |
        (((uint64_t) readQuadWordBytes[7] << 56));

    return true;
}

bool gscaReadString (FILE* fp, char* string, size_t length)
{
    fread(string, sizeof(char), length, fp);
    if (feof(fp))
    {
        gscaErr("Unexpected end-of-file reached while reading string.\n");
        return false;
    }
    else if (ferror(fp))
    {
        gscaErrp("Read error occured while reading string");
        return false;
    }

    string[length] = '\0';
    return true;
}

bool gscaWriteByte (FILE* fp, const uint8_t value)
{
    fwrite(&value, sizeof(uint8_t), 1, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing byte");
        return false;
    }

    return true;
}

bool gscaWriteWord (FILE* fp, const uint16_t value)
{
    static uint8_t writeWordBytes[2] = { 0 };
    writeWordBytes[0] = ((value >> 0) & 0xFF);
    writeWordBytes[1] = ((value >> 8) & 0xFF);

    fwrite(writeWordBytes, sizeof(uint8_t), 2, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing word");
        return false;
    }

    return true;
}

bool gscaWriteDoubleWord (FILE* fp, const uint32_t value)
{
    static uint8_t writeDoubleWordBytes[4] = { 0 };
    writeDoubleWordBytes[0] = ((value >>  0) & 0xFF);
    writeDoubleWordBytes[1] = ((value >>  8) & 0xFF);
    writeDoubleWordBytes[2] = ((value >> 16) & 0xFF);
    writeDoubleWordBytes[3] = ((value >> 24) & 0xFF);

    fwrite(writeDoubleWordBytes, sizeof(uint8_t), 4, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing double word");
        return false;
    }

    return true;
}

bool gscaWriteQuadWord (FILE* fp, const uint64_t value)
{
    static uint8_t writeQuadWordBytes[8] = { 0 };
    writeQuadWordBytes[0] = ((value >>  0) & 0xFF);
    writeQuadWordBytes[1] = ((value >>  8) & 0xFF);
    writeQuadWordBytes[2] = ((value >> 16) & 0xFF);
    writeQuadWordBytes[3] = ((value >> 24) & 0xFF);
    writeQuadWordBytes[4] = ((value >> 32) & 0xFF);
    writeQuadWordBytes[5] = ((value >> 40) & 0xFF);
    writeQuadWordBytes[6] = ((value >> 48) & 0xFF);
    writeQuadWordBytes[7] = ((value >> 56) & 0xFF);

    fwrite(writeQuadWordBytes, sizeof(uint8_t), 8, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing quad word");
        return false;
    }

    return true;
}

bool gscaWriteString (FILE* fp, const char* string, size_t length)
{
    fwrite(string, sizeof(char), length, fp);   // Plus null terminator.
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing string");
        return false;
    }

    return true;
}

/* Public Functions ***********************************************************/

gscaAudioStore* gscaCreateAudioStore (size_t initialCapacity)
{
    gscaAudioStore* audioStore = gscaCreateZero(1, gscaAudioStore);
    gscaExpectp(audioStore, "Could not allocate audio store");
    gscaInitContainers(audioStore, initialCapacity);

    return audioStore;
}

void gscaDestroyAudioStore (gscaAudioStore* audioStore)
{
    if (audioStore != NULL)
    {
        gscaDestroy(audioStore->handles);
        gscaDestroy(audioStore->data);
        gscaDestroy(audioStore);
    }
}

bool gscaReadAudioFile (gscaAudioStore* audioStore, const char* filename)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");
    gscaExpect(filename, "Pointer 'filename' is NULL!\n");

    if (filename[0] == '\0')
    {
        gscaErr("Filename string cannot be blank.\n");
        return false;
    }

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        gscaErrp("Cannot open file '%s' for reading", filename);
        return false;
    }

    // Get and validate file size.
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size < 0)
    {
        gscaErrp("Cannot get size of file '%s'", filename);
        fclose(fp); return false;
    }
    else if (size < sizeof(gscaAudioFileHeader))
    {
        gscaErr("File '%s' is too small.\n", filename);
        fclose(fp); return false;
    }
    rewind(fp);

    // Read header.
    gscaAudioFileHeader header;
    if (
        gscaReadDoubleWord(fp, &header.magicNumber) == false ||
        gscaReadByte(fp, &header.majorVersion) == false ||
        gscaReadByte(fp, &header.minorVersion) == false ||
        gscaReadWord(fp, &header.audioCount) == false
    )
    {
        gscaErr("Could not read header from file '%s'.\n", filename);
        fclose(fp);
        return false;
    }

    // Validate header.
    if (header.magicNumber != GSCA_AS_MAGIC_NUMBER)
    {
        gscaErr("File '%s' has incorrect magic number (0x%08X).\n", filename,
            header.magicNumber);
        fclose(fp); return false;
    }
    else if (header.majorVersion != GSCA_MAJOR_VERSION)
    {
        gscaErr("File '%s' has incorrect major version.\n", filename);
        fclose(fp); return false;
    }
    else if (header.minorVersion > GSCA_MINOR_VERSION)
    {
        gscaErr("File '%s' has incorrect minor version.\n", filename);
        fclose(fp); return false;
    }

    // Load audio handles.
    size_t dataSize = size - sizeof(header);
    for (uint16_t i = 0; i < header.audioCount; ++i)
    {
        gscaResizeHandlesArray(audioStore);
        gscaAudioHandle* handle = &audioStore->handles[audioStore->handlesSize++];
        if (
            gscaReadString(fp, handle->name, GSCA_AS_HANDLE_NAME_STRLEN) == false ||
            gscaReadQuadWord(fp, &handle->offset) == false
        )
        {
            gscaErr("Could not read audio entry #%u from file '%s'.", i, filename);
            fclose(fp); return false;
        }

        handle->offset += audioStore->dataSize;
        handle->id = gscaNextId++;
        dataSize -= (sizeof(handle->name) + sizeof(handle->offset));
    }

    // Load audio data.
    gscaResizeDataBuffer(audioStore, dataSize);
    fread(audioStore->data + audioStore->dataSize, sizeof(uint8_t), dataSize, fp);
    if (feof(fp))
    {
        gscaErr("Unexpected end-of-file reached while reading audio data from file '%s'.\n",
            filename);
        fclose(fp); return false;
    }
    else if (ferror(fp))
    {
        gscaErrp("Read error occured while reading audio data from file '%s'",
            filename);
        fclose(fp); return false;
    }
    
    fclose(fp);
    audioStore->dataSize += dataSize;
    
    return true;
}

bool gscaWriteAudioFile (const gscaAudioStore* audioStore, const char* filename)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");
    gscaExpect(filename, "Pointer 'filename' is NULL!\n");

    if (filename[0] == '\0')
    {
        gscaErr("Filename string cannot be blank.\n");
        return false;
    }

    FILE* fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        gscaErrp("Cannot open file '%s' for writing", filename);
        return false;
    }

    // Write header.
    if (
        gscaWriteDoubleWord(fp, GSCA_AS_MAGIC_NUMBER) == false ||
        gscaWriteByte(fp, GSCA_MAJOR_VERSION) == false ||
        gscaWriteByte(fp, GSCA_MINOR_VERSION) == false ||
        gscaWriteWord(fp, audioStore->handlesSize) == false
    )
    {
        gscaErr("Could not write header to file '%s'.\n", filename);
        fclose(fp);
        return false;
    }

    // Write audio entry table.
    for (size_t i = 0; i < audioStore->handlesSize; ++i)
    {
        const gscaAudioHandle* handle = &audioStore->handles[i];
        if (
            gscaWriteString(fp, handle->name, GSCA_AS_HANDLE_NAME_STRLEN) == false ||
            gscaWriteQuadWord(fp, handle->offset) == false
        )
        {
            gscaErr("Could not write entry #%zu (%s) to entry table of file '%s'.\n",
                i, handle->name, filename);
            fclose(fp);
            return false;
        }
    }

    // Write audio data.
    fwrite(audioStore->data, sizeof(uint8_t), audioStore->dataSize, fp);
    if (ferror(fp))
    {
        gscaErrp("Could not write audio data to file '%s'", filename);
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

const gscaAudioHandle* gscaGetHandleByIndex (const gscaAudioStore* audioStore, size_t index)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");

    for (size_t i = 0; i < audioStore->handlesSize; ++i)
    {
        if (i == index)
        {
            return &audioStore->handles[i];
        }
    }

    return NULL;
}

const gscaAudioHandle* gscaGetHandleByID (const gscaAudioStore* audioStore, uint16_t id)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");

    for (size_t i = 0; i < audioStore->handlesSize; ++i)
    {
        gscaAudioHandle* handle = &audioStore->handles[i];
        if (handle->id == id)
        {
            return handle;
        }
    }

    return NULL;
}

const gscaAudioHandle* gscaGetHandleByName (const gscaAudioStore* audioStore, const char* name)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");
    gscaExpect(name, "Pointer 'name' is NULL!\n");

    for (size_t i = 0; i < audioStore->handlesSize; ++i)
    {
        const gscaAudioHandle* handle = &audioStore->handles[i];
        if (strncmp(handle->name, name, GSCA_AS_HANDLE_NAME_STRLEN) == 0)
        {
            return handle;
        }
    }

    return NULL;
}

const gscaAudioHandle* gscaAddAudio (gscaAudioStore* audioStore, const char* name, const uint8_t* data, uint32_t size)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");
    gscaExpect(name, "Pointer 'name' is NULL!\n");
    gscaExpect(data, "Pointer 'data' is NULL!\n");

    size_t nameLength = strlen(name);
    if (nameLength == 0 || nameLength >= GSCA_AS_HANDLE_NAME_STRLEN)
    {
        gscaErr("Audio handle name must be between 1 and %d in length.\n",
            GSCA_AS_HANDLE_NAME_STRLEN);
        return nullptr;
    }

    const gscaAudioHandle* existingHandle = gscaGetHandleByName(audioStore, name);
    if (existingHandle != NULL)
    {
        return existingHandle;
    }

    if (size == 0)
    {
        gscaErr("Audio entry cannot have zero size.");
        return nullptr;
    }

    gscaResizeHandlesArray(audioStore);
    gscaAudioHandle* handle = &audioStore->handles[audioStore->handlesSize++];
    gscaCopyString(handle->name, name, GSCA_AS_HANDLE_NAME_STRLEN);
    handle->offset = audioStore->dataSize;
    handle->id = gscaNextId++;

    gscaResizeDataBuffer(audioStore, size);
    gscaCopyOffset(audioStore->data, handle->offset, data, 0, size, uint8_t);
    audioStore->dataSize += size;

    return handle;
}

const uint8_t* gscaGetAudioData (const gscaAudioStore* audioStore)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");
    return audioStore->data;
}

const size_t gscaGetAudioDataSize (const gscaAudioStore* audioStore)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");
    return audioStore->dataSize;
}

const size_t gscaGetAudioCount (const gscaAudioStore* audioStore)
{
    gscaExpect(audioStore, "Pointer 'audioStore' is NULL!\n");
    return audioStore->handlesSize;
}
