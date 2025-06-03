/**
 * UEFI T-Rex Runner Game
 *
 * Build: x86_64-w64-mingw32-gcc -O3 -Wall -Wextra -std=c11 -ffreestanding
 *        -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -c trex.c -o trex.o
 *        x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared -Wl,--subsystem,10
 *        -e efi_main -o trex.efi trex.o
 */

#include <stdint.h>
#include <stdbool.h>

/* UEFI types - because microsoft */
#define EFI_SUCCESS 0
#define EFI_INVALID_PARAMETER 2
#define EFI_NOT_READY 6

#define EFI_BLACK   0x00
#define EFI_WHITE   0x0F
#define EFI_GREEN   0x02
#define EFI_RED     0x04
#define EFI_YELLOW  0x0E

#define TRUE  1
#define FALSE 0

#define IN
#define OUT
#define OPTIONAL
#define CONST const
#define EFIAPI __attribute__((ms_abi))
#define NULL ((void*)0)

typedef uint64_t UINTN;
typedef int64_t INTN;
typedef uint64_t EFI_STATUS;
typedef void* EFI_HANDLE;
typedef void* EFI_EVENT;
typedef uint16_t CHAR16;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int16_t INT16;
typedef bool BOOLEAN;
typedef UINTN EFI_TPL;

/* holy shit that's a lot of structs */
typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

/* Time Structure */
typedef struct {
    UINT16 Year;
    UINT8 Month;
    UINT8 Day;
    UINT8 Hour;
    UINT8 Minute;
    UINT8 Second;
    UINT8 Pad1;
    UINT32 Nanosecond;
    INT16 TimeZone;
    UINT8 Daylight;
    UINT8 Pad2;
} EFI_TIME;

/* memory stuff nobody uses but we need anyway */
typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

/* Key Structures */
typedef struct {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

/* Graphics Structures */
typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    UINT32 Version;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK PixelInformation;
    UINT32 PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32 MaxMode;
    UINT32 Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info;
    UINTN SizeOfInfo;
    UINTN FrameBufferBase;
    UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

/* EFI_SIMPLE_TEXT_INPUT_PROTOCOL */
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_STATUS (EFIAPI *Reset)(
        struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
        BOOLEAN ExtendedVerification
    );
    EFI_STATUS (EFIAPI *ReadKeyStroke)(
        struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
        EFI_INPUT_KEY* Key
    );
    EFI_EVENT WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

/* EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL */
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_STATUS (EFIAPI *Reset)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        BOOLEAN ExtendedVerification
    );
    EFI_STATUS (EFIAPI *OutputString)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        CHAR16* String
    );
    EFI_STATUS (EFIAPI *TestString)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        CHAR16* String
    );
    EFI_STATUS (EFIAPI *QueryMode)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        UINTN ModeNumber,
        UINTN* Columns,
        UINTN* Rows
    );
    EFI_STATUS (EFIAPI *SetMode)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        UINTN ModeNumber
    );
    EFI_STATUS (EFIAPI *SetAttribute)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        UINTN Attribute
    );
    EFI_STATUS (EFIAPI *ClearScreen)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This
    );
    EFI_STATUS (EFIAPI *SetCursorPosition)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        UINTN Column,
        UINTN Row
    );
    EFI_STATUS (EFIAPI *EnableCursor)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        BOOLEAN Visible
    );
    void* Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* Graphics Output Protocol */
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    void* QueryMode;
    void* SetMode;
    void* Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

/* Timer Types */
typedef enum {
    TimerCancel,
    TimerPeriodic,
    TimerRelative
} EFI_TIMER_DELAY;

/* Event Types */
#define EVT_TIMER                         0x80000000
#define EVT_RUNTIME                       0x40000000
#define EVT_NOTIFY_WAIT                   0x00000100
#define EVT_NOTIFY_SIGNAL                 0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES     0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

/* Boot Services Table */
typedef struct {
    EFI_TABLE_HEADER Hdr;

    // Task Priority Services
    void* RaiseTPL;  // never used lol
    void* RestoreTPL;

    // Memory Services
    EFI_STATUS (EFIAPI *AllocatePages)(
        UINTN Type,
        EFI_MEMORY_TYPE MemoryType,
        UINTN Pages,
        UINTN* Memory
    );
    void* FreePages;
    void* GetMemoryMap;
    EFI_STATUS (EFIAPI *AllocatePool)(
        EFI_MEMORY_TYPE PoolType,
        UINTN Size,
        void** Buffer
    );
    EFI_STATUS (EFIAPI *FreePool)(
        void* Buffer
    );

    // Event & Timer Services
    EFI_STATUS (EFIAPI *CreateEvent)(  // this one actually works sometimes
        UINT32 Type,
        EFI_TPL NotifyTpl,
        void* NotifyFunction,
        void* NotifyContext,
        EFI_EVENT* Event
    );
    EFI_STATUS (EFIAPI *SetTimer)(
        EFI_EVENT Event,
        EFI_TIMER_DELAY Type,
        UINT64 TriggerTime
    );
    EFI_STATUS (EFIAPI *WaitForEvent)(
        UINTN NumberOfEvents,
        EFI_EVENT* Event,
        UINTN* Index
    );
    void* SignalEvent;
    EFI_STATUS (EFIAPI *CloseEvent)(
        EFI_EVENT Event
    );
    void* CheckEvent;

    // Protocol Handler Services
    void* InstallProtocolInterface;
    void* ReinstallProtocolInterface;
    void* UninstallProtocolInterface;
    void* HandleProtocol;
    void* Reserved;
    void* RegisterProtocolNotify;
    void* LocateHandle;
    void* LocateDevicePath;
    void* InstallConfigurationTable;

    // Image Services
    void* LoadImage;
    void* StartImage;
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;

    // Miscellaneous Services
    void* GetNextMonotonicCount;
    EFI_STATUS (EFIAPI *Stall)(
        UINTN Microseconds
    );
    void* SetWatchdogTimer;

    // DriverSupport Services
    void* ConnectController;
    void* DisconnectController;

    // Open and Close Protocol Services
    void* OpenProtocol;
    void* CloseProtocol;
    void* OpenProtocolInformation;

    // Library Services
    void* ProtocolsPerHandle;
    void* LocateHandleBuffer;
    EFI_STATUS (EFIAPI *LocateProtocol)(
        void* Protocol,
        void* Registration,
        void** Interface
    );
} EFI_BOOT_SERVICES;

/* System Table */
typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16* FirmwareVendor;
    UINT32 FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL* ConIn;
    EFI_HANDLE ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
    EFI_HANDLE StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* StdErr;
    void* RuntimeServices;
    EFI_BOOT_SERVICES* BootServices;
    UINTN NumberOfTableEntries;
    void* ConfigurationTable;
} EFI_SYSTEM_TABLE;

/* GUID Structure */
typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
} EFI_GUID;

/* GUIDs */
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    {0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}}

/* game config */
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   300
#define GROUND_Y        250
#define GRAVITY         1
#define JUMP_VELOCITY   -15
#define GAME_SPEED      8
#define OBSTACLE_GAP    200
#define FRAME_DELAY     20000  // 20ms = 50 FPS (more reasonable for UEFI)

/* Game Structures */
typedef struct {
    INTN x;
    INTN y;
    INTN width;
    INTN height;
    INTN velocityY;
    BOOLEAN isJumping;
    BOOLEAN isDucking;
} Dinosaur;

typedef struct {
    INTN x;
    INTN y;
    INTN width;
    INTN height;
    BOOLEAN active;
} Obstacle;

typedef struct {
    BOOLEAN isRunning;
    BOOLEAN isGameOver;
    UINT32 score;
    UINT32 highScore;
    INTN gameSpeed;
    UINT64 lastFrameTime;
} GameState;

/* dino sprite - looks better than i expected tbh */
static const UINT8 dinoSprite[20][20] = {
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

static const UINT8 cactusSprite[30][15] = {
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,1,0,0,1,1,1,0,0,1,0,0,0,0},
    {0,0,1,0,0,1,1,1,0,0,1,0,0,0,0},
    {0,0,1,0,0,1,1,1,0,0,1,0,0,0,0},
    {0,0,1,0,0,1,1,1,0,0,1,0,0,0,0},
    {0,1,1,1,0,1,1,1,0,1,1,1,0,0,0},
    {0,1,1,1,0,1,1,1,0,1,1,1,0,0,0},
    {0,1,1,1,0,1,1,1,0,1,1,1,0,0,0},
    {0,1,1,1,0,1,1,1,0,1,1,1,0,0,0},
    {0,1,1,1,0,1,1,1,0,1,1,1,0,0,0},
    {0,1,1,1,0,1,1,1,0,1,1,1,0,0,0},
    {0,1,1,1,0,1,1,1,0,1,1,1,0,0,0},
    {0,0,1,1,0,1,1,1,0,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0}
};

/* globals because fuck passing pointers everywhere */
static EFI_SYSTEM_TABLE* gST;
static EFI_BOOT_SERVICES* gBS;
static EFI_GRAPHICS_OUTPUT_PROTOCOL* gGraphicsOutput;
static UINT32* gFrameBuffer;
static UINTN gFrameBufferSize;
static Dinosaur gDino;
static Obstacle gObstacles[5];
static GameState gGameState;

/* memset/memcpy because uefi doesn't give us shit */
static void* memset(void* dest, int val, UINTN count) {
    UINT8* d = dest;
    while (count--) {
        *d++ = (UINT8)val;
    }
    return dest;
}

static void* memcpy(void* dest, const void* src, UINTN count) {
    UINT8* d = dest;
    const UINT8* s = src;
    while (count--) {
        *d++ = *s++;
    }
    return dest;
}

/* int to string - why isn't this built in??? */
static void IntToStr(UINT32 num, CHAR16* str) {
    CHAR16 temp[12];
    int i = 0;

    if (num == 0) {
        str[0] = L'0';
        str[1] = L'\0';
        return;
    }

    while (num > 0) {
        temp[i++] = L'0' + (num % 10);
        num /= 10;
    }

    // reverse the string cuz we built it backwards
    int j = 0;
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = L'\0';
}

/* graphics shit */
static void SetPixel(INTN x, INTN y, UINT32 color) {
    if (x >= 0 && x < (INTN)gGraphicsOutput->Mode->Info->HorizontalResolution &&
        y >= 0 && y < (INTN)gGraphicsOutput->Mode->Info->VerticalResolution) {
        UINT32 pixelsPerScanLine = gGraphicsOutput->Mode->Info->PixelsPerScanLine;
        gFrameBuffer[y * pixelsPerScanLine + x] = color;
    }
}

static void DrawRect(INTN x, INTN y, INTN width, INTN height, UINT32 color) {
    for (INTN j = 0; j < height; j++) {
        for (INTN i = 0; i < width; i++) {
            SetPixel(x + i, y + j, color);
        }
    }
}

static void DrawSprite(INTN x, INTN y, const UINT8* sprite, INTN width, INTN height) {
    UINT32 pixelsPerScanLine = gGraphicsOutput->Mode->Info->PixelsPerScanLine;
    UINT32 screenWidth = gGraphicsOutput->Mode->Info->HorizontalResolution;
    UINT32 screenHeight = gGraphicsOutput->Mode->Info->VerticalResolution;

    for (INTN j = 0; j < height; j++) {
        // Skip if outside screen bounds
        if (y + j < 0 || y + j >= (INTN)screenHeight) continue;

        UINT32 *lineStart = gFrameBuffer + ((y + j) * pixelsPerScanLine);

        for (INTN i = 0; i < width; i++) {
            // bounds check or we crash
            if (x + i < 0 || x + i >= (INTN)screenWidth) continue;

            if (sprite[j * width + i]) {
                lineStart[x + i] = 0x00FFFFFF; // White color
            }
        }
    }
}

static void ClearScreen(UINT32 color) {
    UINT32 width = gGraphicsOutput->Mode->Info->HorizontalResolution;
    UINT32 height = gGraphicsOutput->Mode->Info->VerticalResolution;
    UINT32 pixelsPerScanLine = gGraphicsOutput->Mode->Info->PixelsPerScanLine;

    // fancy pointer math to avoid recalculating offsets
    UINT32 *lineStart = gFrameBuffer;
    for (UINT32 y = 0; y < height; y++) {
        for (UINT32 x = 0; x < width; x++) {
            lineStart[x] = color;
        }
        lineStart += pixelsPerScanLine;
    }
}

static void DrawGround() {
    UINT32 width = gGraphicsOutput->Mode->Info->HorizontalResolution;

    // Draw ground line
    DrawRect(0, GROUND_Y, width, 2, 0x00FFFFFF);

    // ground texture - just some dots lmao
    for (INTN i = 0; i < (INTN)width; i += 20) {
        SetPixel(i + (gGameState.score % 20), GROUND_Y + 5, 0x00808080);
        SetPixel(i + 10 + (gGameState.score % 20), GROUND_Y + 8, 0x00808080);
    }
}

static void DrawScore() {
    CHAR16 scoreStr[32];
    CHAR16 scoreText[64] = L"Score: ";
    CHAR16 highScoreText[64] = L"  High: ";

    // copyright flex
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
    gST->ConOut->OutputString(gST->ConOut, L"github@vladamisici");

    // Convert scores to strings
    IntToStr(gGameState.score, scoreStr);
    int i;
    for (i = 0; scoreText[i]; i++);
    for (int j = 0; scoreStr[j]; j++) {
        scoreText[i++] = scoreStr[j];
    }
    scoreText[i] = L'\0';

    IntToStr(gGameState.highScore, scoreStr);
    for (i = 0; highScoreText[i]; i++);
    for (int j = 0; scoreStr[j]; j++) {
        highScoreText[i++] = scoreStr[j];
    }
    highScoreText[i] = L'\0';

    // Draw score text at top of screen
    gST->ConOut->SetCursorPosition(gST->ConOut, 2, 2);
    gST->ConOut->OutputString(gST->ConOut, scoreText);
    gST->ConOut->OutputString(gST->ConOut, highScoreText);
}

/* game logic */
static void InitGame() {
    // Initialize dinosaur
    gDino.x = 50;
    gDino.y = GROUND_Y - 20;
    gDino.width = 20;
    gDino.height = 20;
    gDino.velocityY = 0;
    gDino.isJumping = false;
    gDino.isDucking = false;

    // Initialize obstacles
    for (int i = 0; i < 5; i++) {
        gObstacles[i].active = false;
    }

    // Initialize game state
    gGameState.isRunning = false;
    gGameState.isGameOver = false;
    gGameState.score = 0;
    gGameState.gameSpeed = GAME_SPEED;
    gGameState.lastFrameTime = 0;
}

static void UpdateDinosaur() {
    if (gDino.isJumping) {
        // gravity goes brrr
        gDino.velocityY += GRAVITY;
        gDino.y += gDino.velocityY;

        // hit the ground
        if (gDino.y >= GROUND_Y - gDino.height) {
            gDino.y = GROUND_Y - gDino.height;
            gDino.velocityY = 0;
            gDino.isJumping = false;
        }
    }
}

static void SpawnObstacle() {
    UINT32 screenWidth = gGraphicsOutput->Mode->Info->HorizontalResolution;

    for (int i = 0; i < 5; i++) {
        if (!gObstacles[i].active) {
            gObstacles[i].x = screenWidth;
            gObstacles[i].y = GROUND_Y - 30;
            gObstacles[i].width = 15;
            gObstacles[i].height = 30;
            gObstacles[i].active = true;
            break;
        }
    }
}

static void UpdateObstacles() {
    static INTN distanceSinceLastObstacle = 0;

    // Move obstacles
    for (int i = 0; i < 5; i++) {
        if (gObstacles[i].active) {
            gObstacles[i].x -= gGameState.gameSpeed;

            // yeet the obstacle offscreen
            if (gObstacles[i].x + gObstacles[i].width < 0) {
                gObstacles[i].active = false;
                gGameState.score += 10;

                // high score tracking
                if (gGameState.score > gGameState.highScore) {
                    gGameState.highScore = gGameState.score;
                }
            }
        }
    }

    // Spawn new obstacles
    distanceSinceLastObstacle += gGameState.gameSpeed;
    if (distanceSinceLastObstacle > OBSTACLE_GAP) {
        SpawnObstacle();
        distanceSinceLastObstacle = 0;
    }

    // make it harder as you go
    if (gGameState.score % 100 == 0 && gGameState.score > 0) {
        gGameState.gameSpeed = GAME_SPEED + (gGameState.score / 100);
    }
}

static BOOLEAN CheckCollision() {
    for (int i = 0; i < 5; i++) {
        if (gObstacles[i].active) {
            // AABB collision check - simple but works
            if (gDino.x < gObstacles[i].x + gObstacles[i].width &&
                gDino.x + gDino.width > gObstacles[i].x &&
                gDino.y < gObstacles[i].y + gObstacles[i].height &&
                gDino.y + gDino.height > gObstacles[i].y) {
                return true;
            }
        }
    }
    return false;
}

static void GameOver() {
    gGameState.isGameOver = true;
    gGameState.isRunning = false;

    // wipe old text
    gST->ConOut->SetCursorPosition(gST->ConOut, 30, 10);
    gST->ConOut->OutputString(gST->ConOut, L"          ");
    gST->ConOut->SetCursorPosition(gST->ConOut, 30, 10);
    gST->ConOut->OutputString(gST->ConOut, L"GAME OVER!");

    gST->ConOut->SetCursorPosition(gST->ConOut, 25, 12);
    gST->ConOut->OutputString(gST->ConOut, L"                        ");
    gST->ConOut->SetCursorPosition(gST->ConOut, 25, 12);
    gST->ConOut->OutputString(gST->ConOut, L"Press SPACE to restart");
}

static void UpdateGame() {
    if (!gGameState.isRunning || gGameState.isGameOver) {
        return;
    }

    UpdateDinosaur();
    UpdateObstacles();

    if (CheckCollision()) {
        GameOver();
    }
}

static void DrawGame() {
    // only redraw the play area - full screen clear is slow af
    UINT32 width = gGraphicsOutput->Mode->Info->HorizontalResolution;
    UINT32 pixelsPerScanLine = gGraphicsOutput->Mode->Info->PixelsPerScanLine;

    // Clear only the game play area (from y=50 to ground)
    for (UINT32 y = 50; y < GROUND_Y + 50; y++) {
        UINT32 *lineStart = gFrameBuffer + (y * pixelsPerScanLine);
        for (UINT32 x = 0; x < width; x++) {
            lineStart[x] = 0x00000000;
        }
    }

    // hackfix for lingering game over text
    if (gGameState.isGameOver) {
        gST->ConOut->SetCursorPosition(gST->ConOut, 30, 10);
        gST->ConOut->OutputString(gST->ConOut, L"                    ");
        gST->ConOut->SetCursorPosition(gST->ConOut, 25, 12);
        gST->ConOut->OutputString(gST->ConOut, L"                              ");
    }

    // Draw ground
    DrawGround();

    // Draw dinosaur
    DrawSprite(gDino.x, gDino.y, (const UINT8*)dinoSprite, 20, 20);

    // Draw obstacles
    for (int i = 0; i < 5; i++) {
        if (gObstacles[i].active) {
            DrawSprite(gObstacles[i].x, gObstacles[i].y,
                      (const UINT8*)cactusSprite, 15, 30);
        }
    }

    // Draw score
    DrawScore();

    // Show instructions if game hasn't started
    if (!gGameState.isRunning && !gGameState.isGameOver) {
        gST->ConOut->SetCursorPosition(gST->ConOut, 25, 15);
        gST->ConOut->OutputString(gST->ConOut, L"Press SPACE to start!");
    }
}

static void HandleInput() {
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;

    // Check for key press
    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    if (Status == EFI_SUCCESS) {
        // Space bar or up arrow
        if (Key.UnicodeChar == L' ' || Key.ScanCode == 0x01) {
            if (!gGameState.isRunning && !gGameState.isGameOver) {
                // Start game
                gGameState.isRunning = true;
            } else if (gGameState.isGameOver) {
                // Restart game
                InitGame();
            } else if (!gDino.isJumping) {
                // Jump
                gDino.isJumping = true;
                gDino.velocityY = JUMP_VELOCITY;
            }
        }
        // ESC key to exit
        else if (Key.ScanCode == 0x17) {
            gGameState.isRunning = false;
            gGameState.isGameOver = true;
        }
    }
}

/* entry point */
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS Status;
    EFI_EVENT TimerEvent;
    UINTN Index;
    EFI_INPUT_KEY Key;

    (void)ImageHandle; // shut up compiler

    // save ptrs
    gST = SystemTable;
    gBS = SystemTable->BootServices;

    // Clear screen
    gST->ConOut->ClearScreen(gST->ConOut);

    // find graphics
    EFI_GUID GraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    Status = gBS->LocateProtocol(&GraphicsOutputProtocolGuid, NULL, (void**)&gGraphicsOutput);
    if (Status != EFI_SUCCESS) {
        gST->ConOut->OutputString(gST->ConOut, L"Failed to locate Graphics Output Protocol\r\n");
        return Status;
    }

    // framebuffer ptr
    gFrameBuffer = (UINT32*)gGraphicsOutput->Mode->FrameBufferBase;
    gFrameBufferSize = gGraphicsOutput->Mode->FrameBufferSize;

    // debug info
    gST->ConOut->OutputString(gST->ConOut, L"Graphics initialized:\r\n");
    CHAR16 modeInfo[100];
    IntToStr(gGraphicsOutput->Mode->Info->HorizontalResolution, modeInfo);
    gST->ConOut->OutputString(gST->ConOut, L"Width: ");
    gST->ConOut->OutputString(gST->ConOut, modeInfo);
    gST->ConOut->OutputString(gST->ConOut, L" x ");
    IntToStr(gGraphicsOutput->Mode->Info->VerticalResolution, modeInfo);
    gST->ConOut->OutputString(gST->ConOut, modeInfo);
    gST->ConOut->OutputString(gST->ConOut, L"\r\n");

    gBS->Stall(2000000); // 2 sec pause to read the info

    // Initialize game
    InitGame();

    // timer setup (doesn't work half the time but whatever)
    // EVT_TIMER flag is required or it fails
    Status = gBS->CreateEvent(0x80000000, 0, NULL, NULL, &TimerEvent); // EVT_TIMER = 0x80000000
    if (Status != EFI_SUCCESS) {
        gST->ConOut->OutputString(gST->ConOut, L"Failed to create timer event\r\n");
        return Status;
    }

    // Set timer to fire every 16.67ms (in 100ns units)
    Status = gBS->SetTimer(TimerEvent, TimerPeriodic, 166700); // 16.67ms = 166700 * 100ns
    if (Status != EFI_SUCCESS) {
        gST->ConOut->OutputString(gST->ConOut, L"Failed to set timer\r\n");
        gST->ConOut->OutputString(gST->ConOut, L"Running without timer...\r\n");
        // fallback to polling mode
    }

    // game loop
    BOOLEAN useTimer = (Status == EFI_SUCCESS);

    ClearScreen(0x00000000);
    gST->ConOut->EnableCursor(gST->ConOut, FALSE); // hide that annoying cursor

    while (true) {
        // input handling
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if (Status == EFI_SUCCESS) {
            // spacebar/up = jump
            if (Key.UnicodeChar == L' ' || Key.ScanCode == 0x01) {
                if (!gGameState.isRunning && !gGameState.isGameOver) {
                    // Start game
                    gGameState.isRunning = true;
                } else if (gGameState.isGameOver) {
                    // restart - wipe screen first
                    ClearScreen(0x00000000);
                    InitGame();
                } else if (!gDino.isJumping) {
                    // Jump
                    gDino.isJumping = true;
                    gDino.velocityY = JUMP_VELOCITY;
                }
            }
            // esc = quit
            else if (Key.ScanCode == 0x17) {
                break;
            }
        }

        // Update game state
        UpdateGame();

        // Draw everything
        DrawGame();

        // 25ms = 40fps - good enough
        gBS->Stall(25000);
    }

    // cleanup
    if (useTimer) {
        gBS->CloseEvent(TimerEvent);
    }
    gST->ConOut->EnableCursor(gST->ConOut, TRUE); // restore cursor
    gST->ConOut->ClearScreen(gST->ConOut);

    return EFI_SUCCESS;
}