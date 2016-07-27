#pragma once

#include <memory>
#include <string>

#include "Macros.h"
#include "typedefs.h"

class FileInfo;
class Log;

// This is needed for compatibility with C++/CLI classes.
// <mutex> is not supported when compiling with /clr or /clr:pure. (for Model.cpp class).
namespace std {
class recursive_mutex;
}

// Main data structure that represents file system. One Index corresponds to one volume.
// Index consists of a tree, that reflects real file system files and directories structure
// and a map from unique file identifier to its FileInfo object.
// Tree is useful for Search logic. Map used for getting a FileInfo by its ID in a constant time.

class Index {

   public:
    explicit Index(char drive_letter);

    NO_COPY(Index)

    ~Index();


    // Returns the ID of the volumes root directory.

    uint RootID() const {
        return root_id_;
    }


    // Sets the ID of the volumes root directory.

    void RootID(uint root_id) {
        root_id_ = root_id;
    }


    // Returns the volumes root directory.

    const FileInfo* Root() const;


    // Yields all stored data ownership and returns the pointer to it.

    std::vector<FileInfo*>* ReleaseData();


    // Sets the volume data (all volume files).

    void SetData(std::unique_ptr<std::vector<FileInfo*>> data);


    // Returns the drive letter for which this index was build.

    char DriveLetter() const {
        return drive_letter_;
    }

    // wstring DriveLetterW(wstring(1, drive_letter)),

    // Returns all files of indexed volume.

    const std::vector<FileInfo*>* LockData() const;


    // Unlocks data after it was locked during AcquireData method call.

    void UnlockData() const;


    // Inserts node into the index.

    void InsertNode(FileInfo* fi) const;


    // Retrieves node from the index.

    FileInfo* GetNode(uint ID) const;


    // Removes node from the index (but not deletes it).
    // The node itself will be deleted as soon as no other objects need this FileInfo.

    void RemoveNode(const FileInfo* node) const;


    // Calculates directories sizes.

    void CalculateDirsSizes() const;


    // For the given file |fi| recursively updates its parent directories sizes till the root.
    // |size_delta| - signed size in KB on which to update.

    void UpdateParentDirsSize(const FileInfo* fi, int size_delta) const;


    // Builds the tree that reflects the real file system files and directories structure.

    void BuildTree() const;

   private:
    char drive_letter_;

    uint root_id_;

    std::unique_ptr<std::vector<FileInfo*>> data_;

    Log* logger_;

    uint64 start_time_;

    std::recursive_mutex* locker_;
};

typedef std::unique_ptr<Index> uIndex;


// Serializes files of the index tree to the Output window, starting from the |first_child|. Used for debugging.

void SerializeToOutput(const FileInfo* first_child, std::wstring indent = L"\t");