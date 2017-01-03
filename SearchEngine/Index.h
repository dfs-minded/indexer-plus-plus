// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>

#include "Macros.h"
#include "typedefs.h"

// This is needed for compatibility with C++/CLI classes.
// <mutex> is not supported when compiling with /clr or /clr:pure. (for Model.cpp class).
namespace std {
	class mutex;
}

namespace indexer_common {
	class FileInfo;
	class Log;
}

namespace indexer {

	// Main data structure that represents file system. One Index corresponds to one volume.
	// Index consists of a tree, that reflects real file system files and directories structure
	// and a map from unique file identifier to its FileInfo object.
	// Tree is useful for Search logic. Map used for getting a FileInfo by its ID in a constant time.

	// Child-Parent relations rules:
	// There must be no nodes in the tree without a parent. During a tree construction, all files
	// without parents will be deleted. During filesystem updates, files with no parents in an index
	// will not be inserted and will be deleted as well.

    class Index {

       public:
        explicit Index(char drive_letter);

        NO_COPY(Index)

        ~Index();


        // Returns the ID of the volumes root directory.

        indexer_common::uint RootID() const {
            return root_id_;
        }


        // Sets the ID of the volumes root directory.

		void RootID(indexer_common::uint root_id) {
            root_id_ = root_id;
        }


        // Returns the volumes root directory.

        const indexer_common::FileInfo* Root() const;


        // Returns the drive letter for which this index was build.

        char DriveLetter() const {
            return drive_letter_;
        }

        std::wstring DriveLetterW() const {
            return drive_letter_w_;
        }


        // Yields all stored data ownership and returns the pointer to it.
        // Assumed that the index data is locked before calling this method.

        std::vector<indexer_common::FileInfo*>* ReleaseData();


        // Sets the volume data (all volume files).
        // Assumed that the index data is locked before calling this method.

        void SetData(std::unique_ptr<std::vector<indexer_common::FileInfo*>> data);


        // Returns all files of the indexed volume. The data is locked until UnlockData() is called.

        const std::vector<indexer_common::FileInfo*>* LockData();


        // Unlocks the data after it was locked during LockData() call.
        // Assumed that the index data is locked before calling this method.

        void UnlockData();


        // Inserts a new node into the index.
        // Returns true if the node was inserted successfully, false otherwise.
        // Assumed that the index data is locked before calling this method.

        bool InsertNode(indexer_common::FileInfo* fi);


        // Retrieves the node from the index.
        // Assumed that the index data is locked before calling this method.

		indexer_common::FileInfo* GetNode(indexer_common::uint ID) const;


        // Removes a node from the index (but not deletes it). Assigns nullptr to all its pointers to the tree
        // except Parent pointer, because we need it to build file paths, which could be requested from UI.
        // The node itself will be deleted as soon as no other objects need or reference this FileInfo.
        // Assumed that the index data is locked before calling this method.

        void RemoveNode(indexer_common::FileInfo* node);


        // Calculates and updates all directories sizes.
        // Assumed that the index data is locked before calling this method.

        void CalculateDirsSizes();


        // For the given file |fi| recursively updates its parent directories sizes till the root.
        // |size_delta| - signed size in KB on which to update.
        // Assumed that the index data is locked before calling this method.

        void UpdateParentDirsSize(const indexer_common::FileInfo* fi, int size_delta);


        // Builds the tree that reflects the real file system files and directories structure. The tree is build using the
        // |data_| member: all FileInfo objects will be assigned corresponding connections to its parent, siblings, first
        // child nodes (if they exist).
        // Assumed that the index data is locked before calling this method.

        void BuildTree();

       private:
        char drive_letter_;

        std::wstring drive_letter_w_;

		indexer_common::uint root_id_;

        std::unique_ptr<std::vector<indexer_common::FileInfo*>> data_;

		indexer_common::Log* logger_;

		indexer_common::uint64 start_time_;

        std::mutex* mtx_;
    };

    typedef std::unique_ptr<Index> uIndex;


	// Serializes files of the index tree to the Output window, starting from the |first_child|. Used for debugging.

    void SerializeToOutput(const indexer_common::FileInfo* first_child, std::wstring indent = L"\t");

} // namespace indexer