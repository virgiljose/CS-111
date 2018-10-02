#!/bin/python

# NAME: Virgil Jose, Christopher Ngai
# EMAIL: xxxxxxxxx, xxxxxxxxx
# ID: xxxxxxxxx, xxxxxxxxx

from __future__ import print_function
import sys
import csv

# global variables
inconsistencies = 0             # hold num of inconsistencies

# Definitions for classes to store information about inodes and directories
class SuperblockInfo():
    def __init__(self, num_blocks=0, num_inodes=0, size_blocks=0, size_inodes=0, blocks_group=0, inodes_group=0, first_nr_inode=0, first_block_inodes=0):
        self.num_blocks = num_blocks
        self.num_inodes = num_inodes
        self.size_blocks = size_blocks
        self.size_inodes = size_inodes
        self.blocks_group = blocks_group
        self.inodes_group = inodes_group
        self.first_nr_inode = first_nr_inode
        self.first_block_inodes = first_block_inodes

class BlockInfo():
    def __init__(self, block_num=0, inode_num=0, offset=0, level=0):
        self.block_num = block_num
        self.inode_num = inode_num
        self.offset = offset
        self.level = level

class InodeInfo():
    def __init__(self, inode_num=0, inode_mode=0, link_count=0):
        self.inode_num = inode_num
        self.inode_mode = inode_mode
        self.link_count = link_count
        self.links_found = 0
        self.addresses = []

class DirInfo():
    def __init__(self, parent_inode_num=0, ref_inode_num=0, name_entry=0):
        self.parent_inode_num = parent_inode_num
        self.ref_inode_num = ref_inode_num
        self.name_entry = name_entry


# Array/List/Dict of the above classes ^
superblock = SuperblockInfo()   # initialize SuperblockInfo class
freeBlocks = []                 # hold all free blocks (BFREE)
freeInodes = []                 # hold all free inodes (IFREE)
blockDict = dict()              # hold all blocks
inodeDict = dict()              # store each inode, with key being the inode # and the value being the inode class instance
pinDict = dict()                # hold parent inode numbers
listDirs = []                   # hold all directory entries

# WRAPPER FUNCTIONS
# BLOCK CONSISTENCY AUDITS
def checkBlocks():
    global inconsistencies

    start = superblock.first_block_inodes + (128 * superblock.num_inodes - 1) / superblock.size_blocks + 1
    # iterate through all the blocks
    for i in blockDict.keys():
        block = list(blockDict[i])
        realBlock = block[0]
        block_num = realBlock.block_num
        block_level = realBlock.level
        block_type = "BLOCK"
        if block_level == 1:
            block_type = "INDIRECT BLOCK"
        if block_level == 2:
            block_type = "DOUBLE INDIRECT BLOCK"
        if block_level == 3:
            block_type = "TRIPLE INDIRECT BLOCK"
        # check if the block is valid
        if i < 0 or i >= superblock.num_blocks:
            print("INVALID %s %d IN INODE %d AT OFFSET %d" % (block_type, block_num, realBlock.inode_num, realBlock.offset))
            inconsistencies += 1
        # check if block is free
        elif block_num in freeBlocks:
            print("ALLOCATED BLOCK %d ON FREELIST" % block_num)
            inconsistencies += 1
        # check if block is reserved
        elif i < start:
            print("RESERVED %s %d IN INODE %d AT OFFSET %d" % (block_type, block_num, realBlock.inode_num, realBlock.offset))
            inconsistencies += 1
        # check if block is duplicated
        elif len(block) > 1:
            for duplicates in block:
                block_level = duplicates.level
                block_type = "BLOCK"
                if block_level == 1:
                    block_type = "INDIRECT BLOCK"
                if block_level == 2:
                    block_type = "DOUBLE INDIRECT BLOCK"
                if block_level == 3:
                    block_type = "TRIPLE INDIRECT BLOCK"
                print("DUPLICATE %s %d IN INODE %d AT OFFSET %d" % (block_type, block_num, duplicates.inode_num, duplicates.offset))
                inconsistencies += 1
        

    # check for free legal data blocks
    # legal data block = every block between end of inodes and start of next group
    for j in range(start + 1, superblock.num_blocks):
        #check if legal
        if j not in freeBlocks and j not in blockDict.keys():
            print("UNREFERENCED BLOCK %d" % (j))
            inconsistencies += 1

# wrapper function for DCA to check links found
def checkLinks():
    global inconsistencies

    for key in inodeDict:
        if inodeDict[key].inode_mode > 0 and inodeDict[key].link_count != inodeDict[key].links_found:
            print("INODE %d HAS %d LINKS BUT LINKCOUNT IS %d" % (inodeDict[key].inode_num, inodeDict[key].links_found, inodeDict[key].link_count))
            inconsistencies += 1

# DIECTORY CONSISTENCY AUDIT
def checkDirs():
    global inconsistencies

    for i in listDirs:
        # check if invalid inode
        if i.ref_inode_num < 1 or i.ref_inode_num > superblock.num_inodes:
            print("DIRECTORY INODE %d NAME %s INVALID INODE %d" % (i.parent_inode_num, i.name_entry, i.ref_inode_num))
            inconsistencies += 1
            continue

        #update link count
        if i.ref_inode_num in inodeDict.keys():
            inodeDict[i.ref_inode_num].links_found += 1

        #check current directory
        if i.name_entry == '\'.\'':
            if i.parent_inode_num != i.ref_inode_num:
                print("DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d" % (i.parent_inode_num, i.name_entry, i.ref_inode_num, i.parent_inode_num))
                inconsistencies += 1
        #check parent directory
        elif i.name_entry == '\'..\'':
            if i.parent_inode_num == 2 or i.ref_inode_num == 2:
                gp_inode_num = 2
            else:
                gp_inode_num = pinDict[i.parent_inode_num]

            #check grandparent directory
            if gp_inode_num != i.ref_inode_num:
                print("DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d" % (i.parent_inode_num, i.name_entry, i.ref_inode_num, gp_inode_num))
                inconsistencies += 1

        #check if unallocated
        elif i.ref_inode_num in freeInodes:
            #if i.ref_inode_num >= superblock.first_nr_inode and i.ref_inode_num <= num_blocks:
            print("DIRECTORY INODE %d NAME %s UNALLOCATED INODE %d" % (i.parent_inode_num, i.name_entry, i.ref_inode_num))
            inconsistencies += 1
        elif i.ref_inode_num in inodeDict.keys() and inodeDict[i.ref_inode_num].inode_mode <= 0:
            print("DIRECTORY INODE %d NAME %s UNALLOCATED INODE %d" % (i.superblock, i.name_entry, i.ref_inode_num))
            inconsistencies += 1
        elif i.ref_inode_num not in inodeDict.keys() and i.ref_inode_num > superblock.first_nr_inode:
            print("DIRECTORY INODE %d NAME %s UNALLOCATED INODE %d" % (i.parent_inode_num, i.name_entry, i.ref_inode_num))
            inconsistencies += 1

    #check if reference count matches link count
    checkLinks()

# I-NODE ALLOCATION AUDIT
def checkInodes():
    global inconsistencies
    
    for i in inodeDict.keys():
        inode = inodeDict[i]
        if inode.inode_mode > 0 and inode.link_count > 0:
            if inode.inode_num in freeInodes:
                print("ALLOCATED INODE %d ON FREELIST" % inode.inode_num)
                inconsistencies += 1
            elif inode.inode_mode < 0 and inode.inode_num not in freeInodes:
                print("UNALLOCATED INODE %d NOT ON FREELIST" % inode.inode_num)
                inconsistencies += 1

    # check if unallocated            
    for i in range(superblock.first_nr_inode, superblock.num_inodes+1):
        if i not in freeInodes and i not in inodeDict.keys():
            print("UNALLOCATED INODE %d NOT ON FREELIST" % i)
            inconsistencies += 1

def parse_csv_file():
    # check if we have the correct number of arguments
    if len(sys.argv) != 2:
        print("Invalid number of arguments. Usage: ./lab3b.py csvfile.csv", file=sys.stderr)
        sys.exit(1)
    
    # open the CSV file
    try:
        csvFile = open(sys.argv[1], "rb")
    except:
        print("Error opening CSV file", file=sys.stderr)
        sys.exit(1)

    # parse the CSV file
    parser = csv.reader(csvFile, delimiter=',')

    # go through every line in ther CSV file
    for row in parser:
        if row[0] == 'SUPERBLOCK':
            superblock.num_blocks = int(row[1])
            superblock.num_inodes = int(row[2])
            superblock.size_blocks = int(row[3])
            superblock.size_inodes = int(row[4])
            superblock.blocks_group = int(row[5])
            superblock.inodes_group = int(row[6])
            superblock.first_nr_inode = int(row[7])

        if row[0] == 'GROUP':
            superblock.first_block_inodes = int(row[7])
            
        if row[0] == 'BFREE':
            freeBlocks.append(int(row[1]))

        if row[0] == 'IFREE':
            freeInodes.append(int(row[1]))

        if row[0] == 'INODE':
            inode = InodeInfo(int(row[1]),int(row[3]),int(row[6]))
            
            # iterate through each block addresses
            for x in range(15):
                block_addrs = int(row[12+x])
                inode.addresses.append(block_addrs)
                level = 0
                offset = x
                
                # if single, double, triple indirect block
                if x >= 12:
                    #level will be either 1, 2, 3
                    level = x - 11

                    # set original offset
                    if level == 1:
                        offset = 12
                    elif level == 2:
                        offset = 268
                    elif level == 3:
                        offset = 65804
                        
                # create BlockInfo object and add to blockDict
                if block_addrs > 0:
                    block = BlockInfo(block_addrs, int(row[1]), offset, level)

                    #check if block already in blockDict
                    if block_addrs not in blockDict:
                        blockDict[block_addrs] = set()
                    blockDict[block_addrs].add(block)

            # append inode to inodeDict
            inodeDict[int(row[1])] = inode
        
        if row[0] == 'DIRENT':
            name_entry = row[6]
            direct = DirInfo(int(row[1]), int(row[3]), name_entry)
            listDirs.append(direct)
            if name_entry != '\'.\'' and name_entry != '\'..\'':
                pinDict[int(row[3])] = int(row[1])

        if row[0] == 'INDIRECT':
            block_addrs = int(row[5])
            block = BlockInfo(block_addrs, int(row[1]), int(row[3]), int(row[2]) - 1)

            #check if block already in blockDict
            if block_addrs not in blockDict:
                blockDict[block_addrs] = set()
            blockDict[block_addrs].add(block)

if __name__ == "__main__":
    parse_csv_file()
    checkBlocks()
    checkDirs()
    checkInodes()

    if inconsistencies != 0:
        sys.exit(2)
    else:
        sys.exit(0)
