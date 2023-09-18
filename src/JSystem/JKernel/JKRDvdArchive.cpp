//
// Generated by dtk
// Translation Unit: JKRDvdArchive.cpp
//

#include "JSystem/JKernel/JKRDvdArchive.h"
#include "JSystem/JKernel/JKRDvdFile.h"
#include "JSystem/JKernel/JKRDvdRipper.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "MSL_C/math.h"
#include "MSL_C/string.h"
#include "dolphin/os/OS.h"

/* 802BABD4-802BAC7C       .text __ct__13JKRDvdArchiveFlQ210JKRArchive15EMountDirection */
JKRDvdArchive::JKRDvdArchive(s32 entryNum, JKRArchive::EMountDirection mountDirection) : JKRArchive(entryNum, MOUNT_DVD) {
    mMountDirection = mountDirection;
    if (!open(entryNum))
        return;

    mVolumeType = 'RARC';
    mVolumeName = mStringTable + mNodes->name_offset;
    getVolumeList().prepend(&mFileLoaderLink);
    mIsMounted = true;
}

/* 802BAC7C-802BAD98       .text __dt__13JKRDvdArchiveFv */
JKRDvdArchive::~JKRDvdArchive() {
    if (mIsMounted == true) {
        if (mArcInfoBlock) {
            SDIFileEntry* fileEntry = mFiles;
            int i = 0;
            for (; i < mArcInfoBlock->num_file_entries; fileEntry++, i++) {
                if (fileEntry->data) {
                    JKRFreeToHeap(mHeap, fileEntry->data);
                }
            }

            JKRFreeToHeap(mHeap, mArcInfoBlock);
            mArcInfoBlock = NULL;
        }

        if (mExpandedSize) {
            i_JKRFree(mExpandedSize);
            mExpandedSize = NULL;
        }

        if (mDvdFile) {
            delete mDvdFile;
        }

        getVolumeList().remove(&mFileLoaderLink);
        mIsMounted = false;
    }
}

static void dummy() {
    OSReport(__FILE__);
    OSReport("isMounted()");
    OSReport("Halt");
    OSReport("mMountCount == 1");
}

/* 802BAD98-802BB024       .text open__13JKRDvdArchiveFl */
bool JKRDvdArchive::open(s32 entryNum) {
    mArcInfoBlock = NULL;
    field_0x64 = NULL;
    mNodes = NULL;
    mFiles = NULL;
    mStringTable = NULL;

    mDvdFile = new (JKRGetSystemHeap(), 0) JKRDvdFile(entryNum);
    if (!mDvdFile) {
        mMountMode = UNKNOWN_MOUNT_MODE;
        return false;
    }

    SArcHeader* arcHeader = (SArcHeader*)JKRAllocFromSysHeap(sizeof(SArcHeader), 0x20);
    if (!arcHeader) {
        mMountMode = UNKNOWN_MOUNT_MODE;
        goto cleanup;
    }

    JKRDvdToMainRam(entryNum, (u8*)arcHeader, EXPAND_SWITCH_UNKNOWN1, sizeof(SArcHeader), NULL,
                    JKRDvdRipper::ALLOC_DIRECTION_FORWARD, 0, &mCompression);

    int alignment = mMountDirection == MOUNT_DIRECTION_HEAD ? 0x20 : -0x20;

    mArcInfoBlock = (SArcDataInfo*)JKRAllocFromHeap(mHeap, arcHeader->file_data_offset, alignment);
    if (!mArcInfoBlock) {
        mMountMode = UNKNOWN_MOUNT_MODE;
        goto cleanup;
    }

    JKRDvdToMainRam(entryNum, (u8*)mArcInfoBlock, EXPAND_SWITCH_UNKNOWN1,
                    arcHeader->file_data_offset, NULL, JKRDvdRipper::ALLOC_DIRECTION_FORWARD,
                    sizeof(SArcHeader), NULL);

    mNodes = (SDIDirEntry*)((int)&mArcInfoBlock->num_nodes + mArcInfoBlock->node_offset);
    mFiles = (SDIFileEntry*)((int)&mArcInfoBlock->num_nodes + mArcInfoBlock->file_entry_offset);
    mStringTable = (char*)((int)&mArcInfoBlock->num_nodes + mArcInfoBlock->string_table_offset);
    mExpandedSize = NULL;

    u8 useCompression = 0;
    SDIFileEntry* fileEntry = mFiles;
    for (u32 i = 0; i < mArcInfoBlock->num_file_entries; fileEntry++, i++) {
        if (fileEntry->isUnknownFlag1()) {
            useCompression |= fileEntry->getCompressFlag();
        }
    }

    if (useCompression) {
        mExpandedSize = (s32*)JKRAllocFromHeap(mHeap, sizeof(s32) * mArcInfoBlock->num_file_entries,
                                               abs(alignment));
        if (!mExpandedSize) {
            // !@bug: mArcInfoBlock is allocated from mHeap but free'd to sSystemHeap. I don't know
            // what will happen if mHeap != sSystemHeap, but it's still a bug to free to the wrong
            // allocator.
            JKRFreeToSysHeap(mArcInfoBlock);
            mMountMode = UNKNOWN_MOUNT_MODE;
            goto cleanup;
        }

        memset(mExpandedSize, 0, sizeof(s32) * mArcInfoBlock->num_file_entries);
    }

    field_0x64 = arcHeader->header_length + arcHeader->file_data_offset;

cleanup:
    if (arcHeader) {
        JKRFreeToSysHeap(arcHeader);
    }

    if (mMountMode == UNKNOWN_MOUNT_MODE) {
        OSReport(":::Cannot alloc memory [%s][%d]\n", __FILE__, 382);
        if (mDvdFile) {
            delete mDvdFile;
        }
        return false;
    }

    return true;
}

/* 802BB024-802BB17C       .text fetchResource__13JKRDvdArchiveFPQ210JKRArchive12SDIFileEntryPUl */
void* JKRDvdArchive::fetchResource(SDIFileEntry* fileEntry, u32* returnSize) {
    JUT_ASSERT(413, isMounted());
    u32 tempReturnSize;
    if (returnSize == NULL) {
        returnSize = &tempReturnSize;
    }

    JKRCompression fileCompression = JKRConvertAttrToCompressionType(fileEntry->getAttr());
    if (!fileEntry->data) {
        u8* resourcePtr;
        u32 resourceSize = fetchResource_subroutine(
            mEntryNum, this->field_0x64 + fileEntry->data_offset, fileEntry->data_size, mHeap,
            fileCompression, mCompression, &resourcePtr);
        *returnSize = resourceSize;
        if (resourceSize == 0) {
            return NULL;
        }

        fileEntry->data = resourcePtr;
        if (fileCompression == COMPRESSION_YAZ0) {
            setExpandSize(fileEntry, *returnSize);
        }
    } else {
        if (fileCompression == COMPRESSION_YAZ0) {
            u32 resourceSize = getExpandSize(fileEntry);
            *returnSize = resourceSize;
        } else {
            *returnSize = fileEntry->data_size;
        }
    }

    return fileEntry->data;
}

/* 802BB17C-802BB2BC       .text fetchResource__13JKRDvdArchiveFPvUlPQ210JKRArchive12SDIFileEntryPUl */
void* JKRDvdArchive::fetchResource(void* buffer, u32 bufferSize, SDIFileEntry* fileEntry, u32* returnSize) {
    /* Nonmatching */
    JUT_ASSERT(489, isMounted());
    u32 otherSize;
    u32 size = fileEntry->data_size;
    JKRCompression fileCompression = JKRConvertAttrToCompressionType(fileEntry->getAttr());

    if (!fileEntry->data) {
        bufferSize = ALIGN_PREV(bufferSize, 0x20);
        size = fetchResource_subroutine(mEntryNum, field_0x64 + fileEntry->data_offset,
                                        fileEntry->data_size, (u8*)buffer, bufferSize, fileCompression,
                                        mCompression);
    } else {
        if (fileCompression == COMPRESSION_YAZ0) {
            otherSize = getExpandSize(fileEntry);
            if (otherSize) {
                size = otherSize;
            }
        }

        if (size > bufferSize) {
            size = bufferSize;
        }

        JKRHeap::copyMemory(buffer, fileEntry->data, size);
    }

    if (returnSize) {
        *returnSize = size;
    }
    return buffer;
}

/* 802BB2BC-802BB47C       .text fetchResource_subroutine__13JKRDvdArchiveFlUlUlPUcUlii */
u32 JKRDvdArchive::fetchResource_subroutine(s32 entryNum, u32 offset, u32 size, u8* dst, u32 dstLength, int fileCompression, int archiveCompression) {
    u32 alignedSize = ALIGN_NEXT(size, 0x20);
    u32 alignedDstLength = ALIGN_PREV(dstLength, 0x20);

    switch (archiveCompression) {
    case COMPRESSION_NONE: {
        switch (fileCompression) {
        case COMPRESSION_NONE:
            if (alignedSize > alignedDstLength) {
                alignedSize = alignedDstLength;
            }

            JKRDvdToMainRam(entryNum, dst, EXPAND_SWITCH_UNKNOWN0, alignedSize, NULL,
                            JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);
            return alignedSize;
        case COMPRESSION_YAY0:
        case COMPRESSION_YAZ0:
            // The dst pointer to JKRDvdToMainRam should be aligned to 32 bytes. This will align
            // arcHeader to 32 bytes on the stack.
            char arcHeaderBuffer[64];
            u8* arcHeader = (u8*)ALIGN_NEXT((u32)arcHeaderBuffer, 0x20);
            JKRDvdToMainRam(entryNum, arcHeader, EXPAND_SWITCH_UNKNOWN2, sizeof(SArcHeader),
                            NULL, JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);
            DCInvalidateRange(arcHeader, sizeof(SArcHeader));

            u32 decompressedSize = JKRDecompExpandSize(arcHeader);
            u32 alignedDecompressedSize = ALIGN_NEXT(decompressedSize, 0x20);
            if (alignedDecompressedSize > alignedDstLength) {
                alignedDecompressedSize = alignedDstLength;
            }

            JKRDvdToMainRam(entryNum, dst, EXPAND_SWITCH_UNKNOWN1, alignedDecompressedSize, NULL,
                            JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);
            return decompressedSize;
        }
    }

    case COMPRESSION_YAZ0: {
        if (size > alignedDstLength) {
            size = alignedDstLength;
        }

        JKRDvdToMainRam(entryNum, dst, EXPAND_SWITCH_UNKNOWN1, size, NULL,
                        JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);
        return size;
    }

    case COMPRESSION_YAY0: {
        OSPanic(__FILE__, 603, "Sorry, not prepared for SZP archive.\n");
        return 0;
    }

    default: {
        OSPanic(__FILE__, 609, ":::??? bad sequence\n");
        return 0;
    }
    }
}

/* 802BB47C-802BB6F8       .text fetchResource_subroutine__13JKRDvdArchiveFlUlUlP7JKRHeapiiPPUc */
u32 JKRDvdArchive::fetchResource_subroutine(s32 entryNum, u32 offset, u32 size, JKRHeap* heap, int fileCompression, int archiveCompression, u8** returnResource) {
    u32 alignedSize = ALIGN_NEXT(size, 0x20);
    u8* buffer;

    switch (archiveCompression) {
    case COMPRESSION_NONE: {
        switch (fileCompression) {
        case COMPRESSION_NONE:
            buffer = (u8*)JKRAllocFromHeap(heap, alignedSize, sizeof(SArcHeader));
            JUT_ASSERT(631, buffer != 0);

            JKRDvdToMainRam(entryNum, buffer, EXPAND_SWITCH_UNKNOWN0, alignedSize, NULL,
                            JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);

            *returnResource = buffer;
            return alignedSize;
        case COMPRESSION_YAY0:
        case COMPRESSION_YAZ0:
            // The dst pointer to JKRDvdToMainRam should be aligned to 32 bytes. This will align
            // arcHeader to 32 bytes on the stack.
            char arcHeaderBuffer[64];
            u8* arcHeader = (u8*)ALIGN_NEXT((u32)arcHeaderBuffer, 0x20);
            JKRDvdToMainRam(entryNum, arcHeader, EXPAND_SWITCH_UNKNOWN2, sizeof(SArcHeader),
                            NULL, JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);
            DCInvalidateRange(arcHeader, sizeof(SArcHeader));

            alignedSize = JKRDecompExpandSize(arcHeader);
            buffer = (u8*)JKRAllocFromHeap(heap, alignedSize, sizeof(SArcHeader));
            JUT_ASSERT(660, buffer);
            JKRDvdToMainRam(entryNum, buffer, EXPAND_SWITCH_UNKNOWN1, alignedSize, NULL,
                            JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);

            *returnResource = buffer;
            return alignedSize;
        }
    }

    case COMPRESSION_YAZ0: {
        buffer = (u8*)JKRAllocFromHeap(heap, alignedSize, sizeof(SArcHeader));
        JUT_ASSERT(672, buffer);

        JKRDvdToMainRam(entryNum, buffer, EXPAND_SWITCH_UNKNOWN1, size, NULL,
                        JKRDvdRipper::ALLOC_DIRECTION_FORWARD, offset, NULL);

        *returnResource = buffer;
        return alignedSize;
    }

    case COMPRESSION_YAY0: {
        OSPanic(__FILE__, 683, "Sorry, not prepared for SZP archive.\n");
        return 0;
    }

    default: {
        OSPanic(__FILE__, 688, ":::??? bad sequence\n");
        return 0;
    }
    }
}

/* 802BB6F8-802BB82C       .text getExpandedResSize__13JKRDvdArchiveCFPCv */
u32 JKRDvdArchive::getExpandedResSize(const void* resource) const {
    u32 resourceSize;
    if (!mExpandedSize) {
        return getResSize(resource);
    }

    SDIFileEntry* fileEntry = findPtrResource(resource);
    if (!fileEntry) {
        return -1;
    }

    if (!fileEntry->isCompressed()) {
        return getResSize(resource);
    }

    resourceSize = getExpandSize(fileEntry);
    if (resourceSize) {
        return resourceSize;
    }

    // The dst pointer to JKRDvdToMainRam should be aligned to 32 bytes. This will align arcHeader
    // to 32 bytes on the stack.
    char buffer[64];
    u8* arcHeader = (u8*)ALIGN_NEXT((u32)buffer, 0x20);
    JKRDvdToMainRam(mEntryNum, arcHeader, EXPAND_SWITCH_UNKNOWN2, sizeof(SArcHeader), NULL,
                    JKRDvdRipper::ALLOC_DIRECTION_FORWARD,
                    this->field_0x64 + fileEntry->data_offset, NULL);
    DCInvalidateRange(arcHeader, sizeof(SArcHeader));

    resourceSize = JKRDecompExpandSize(arcHeader);
    // ???
    ((JKRDvdArchive*)this)->setExpandSize(fileEntry, resourceSize);

    return resourceSize;
}
