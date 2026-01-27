#!/usr/bin/env python3

## \file
#  Unit tests for Mille python3 bindings
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



# In the CI, we do not have the Mille module in place yet 
try: 
    import milleWriter, milleReader 
except ModuleNotFoundError:
    # this is the import expected to happen outside the CI
    try: 
        from Mille import milleWriter, milleReader 
    except ModuleNotFoundError:
        raise

import numpy as np 
from pprint import pprint 

class testPayload:
    def __init__(self):
        self.meas = 0.03
        self.sigma = 1e-4
        self.localLabels = np.array([1,2,3], dtype=np.uint32)
        self.localDeriv = np.array([-1.,1.,0.5], dtype=np.float64)
        self.globalLabels = np.array([1001,2001,2003,4000], dtype=np.int32)
        self.globalDeriv = np.array([-.2,13.,0.5,-1.], dtype=np.float64)
        self.specialInt = np.array([3,4], dtype=np.int32)
        self.specialFloat = np.array([42.,-99.], dtype=np.float64)

    def expectedBufferInt(self):
        exp = np.append(np.zeros(2,dtype=np.int32),self.localLabels)
        exp = np.append(exp,np.zeros(1,dtype=np.int32))
        exp = np.append(exp,self.globalLabels)
        exp = np.append(exp,np.zeros(2,dtype=np.int32))
        exp = np.append(exp,self.specialInt)
        return exp

    def expectedBufferDouble(self):
        exp = np.append(np.array([0,self.meas],dtype=np.float64),self.localDeriv)
        exp = np.append(exp, np.array([self.sigma],dtype=np.float64))
        exp = np.append(exp, self.globalDeriv)
        exp = np.append(exp, np.array([0, -1 * len(self.specialInt)],dtype=np.float64))
        exp = np.append(exp, self.specialFloat)
        return exp


def checkExact(left, right, testName):
    if np.all(left == right):
        return True
    else:
        print (f"FAILED test {testName}")
        pprint (left)
        pprint(right)
        pprint(left==right)

def checkEpsi(left, right, testName, epsilon = 1e-6):
    if np.all(np.abs(left - right)<epsilon):
        return True
    else:
        print (f"FAILED test {testName}")
        pprint (left)
        pprint(right)
        pprint(np.abs(left - right))


if __name__ == "__main__": 
    test = testPayload()
    writer = milleWriter.milleWriter("testFromPy3.root")
    writer.addData(test.meas, test.sigma, 
                    test.localLabels,
                    test.localDeriv,
                    test.globalLabels,
                    test.globalDeriv) 
    writer.addSpecial(test.specialInt, test.specialFloat) 
    writer.write()
    writer.close()

    reader = milleReader.milleReader()
    stat = reader.open("testFromPy3.root") 
    success = checkExact(stat,0,"Reader open return code")
    
    stat, ints, doubles = reader.readRecord()
    success = success and checkExact(stat,4,"readRecord return")

    success = success and checkExact(ints, test.expectedBufferInt(), "Int payload")
    success = success and checkEpsi(doubles, test.expectedBufferDouble(), "Float payload")

    stat, ints, doubles = reader.readRecord()
    success = success and checkExact(stat,0,"readRecord return at EOF")

    exit(0 if success else 1 )
    

