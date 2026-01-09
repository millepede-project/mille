## \file
#  Example for Mille format conversion in python 
#
# \author Maximilian Goblirsch-Kolb, DESY (2026) 
#
#  \copyright
#  Copyright (c) 2026 Deutsches Elektronen-Synchroton,
#  Member of the Helmholtz Association, (DESY), HAMBURG, GERMANY \n\n
#  This library is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Library General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version. \n\n
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Library General Public License for more details. \n\n
#  You should have received a copy of the GNU Library General Public
#  License along with this program (see the file COPYING.LIB for more
#  details); if not, write to the Free Software Foundation, Inc.,
#  675 Mass Ave, Cambridge, MA 02139, USA.
#
#

import milleWriter, milleReader 

if __name__ == "__main__":
    from argparse import ArgumentParser 

    parser = ArgumentParser("hi")
    parser.add_argument("outFile",type=str, help="Output Mille file to write")
    parser.add_argument("inFiles",type=str,nargs="+",help="One or many input Mille files")
    parser.add_argument("-n",dest="maxRecords",type=int, default=-1, help="max. number of records to copy")
    parser.add_argument("-s",dest="skipRecords",type=int, default=-1, help="number of records to skip before copying")

    args=parser.parse_args()

    # schedule a MilleRecord to write our output file. 
    # The type is automatically inferred from the file extension
    out = milleWriter.milleWriter(args.outFile) 

    # Instantiate a MilleReader 
    reader = milleReader.milleReader()

    # Counters to allow reading a subset of the records
    nRead = 0
    nSkipped = 0

    # loop over input files  
    for f in args.inFiles:
        nStart = nRead 
        # Load the next file 
        if (reader.open(f) != 0):
            print ("Failed to read ",f)
            continue 
        stat = 1 
        # Loop over records. Status 0 indicates EoF
        while stat > 0: 
            # handle skipping first N records
            if (args.skipRecords > 0):
                args.skipRecords -=1 
                reader.skipRecord()
                continue 
            # handle max number of records to copy 
            if (args.maxRecords >= 0 and nRead >= args.maxRecords):
                break   
            # read the encoded buffer content into numpy arrays 
            stat, ints, doubles = reader.readRecord()
            # check status - increment succesful reads
            if (stat > 0): 
                nRead += 1
            # write our arrays into the output buffers 
            out.copyToBuffer(ints,doubles)
            # and write to the output Mille file 
            out.write()
        # Done with this file - clean up 
        print ("Done with ",f," - copied ",nRead - nStart," entries")
        reader.close()
        # flush output to disk after each input file
        out.flush()
    # All done, finalise the output 
    out.close()
