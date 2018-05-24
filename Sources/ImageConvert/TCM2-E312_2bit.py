"""
Copyright (c) 2018, MpicoSys
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""


__copyright__ = "Copyright (C) MpicoSys-Embedded Pico Systems"
__author__ = "Andrzej Majchrzak <andrzej.majchrzak@mpicosys.com>"
__version__ = "1.0"

from PIL import Image
import cStringIO
import sys, getopt
import os
import os.path

class TCM2312:
    """
    Class used to convert images TCM2 
    """
    __tc__ = "TCM22-E312-320"
    filename_prefix = "E312_"
    filename_sufix = "_2bit"

    def __init__(self):
        self.imagedata = None

    def get_epd_header(self):
        # EPD file format
        return b'\x3F\x05\xA0\x0A\x00\x02\x07'+('\x00'*9)

    def rawImg2binData(self, imgbytes):
        imgnewbytes0 = list((len(imgbytes) / 8) * b'\x00')
        imgnewbytes1 = list((len(imgbytes) / 8) * b'\x00')

        pointer = 0
        i = 0
        loop_len = len(imgbytes)
        while i < loop_len:
            temp = (((imgbytes[i + 0]&1) << 7) & 0x80) | \
                   (((imgbytes[i + 1]&1) << 6) & 0x40) | \
                   (((imgbytes[i + 2]&1) << 5) & 0x20) | \
                   (((imgbytes[i + 3]&1) << 4) & 0x10) | \
                   (((imgbytes[i + 4]&1) << 3) & 0x08) | \
                   (((imgbytes[i + 5]&1) << 2) & 0x04) | \
                   (((imgbytes[i + 6]&1) << 1) & 0x02) | \
                   (((imgbytes[i + 7]&1) << 0) & 0x01)

            imgnewbytes0[pointer] = chr(temp & 0xFF)

            pointer += 1
            i += 8

        pointer = 0
        i = 0
        loop_len = len(imgbytes)
        while i < loop_len:
            temp = (((imgbytes[i + 0]&2) << 6) & 0x80) | \
                   (((imgbytes[i + 1]&2) << 5) & 0x40) | \
                   (((imgbytes[i + 2]&2) << 4) & 0x20) | \
                   (((imgbytes[i + 3]&2) << 3) & 0x10) | \
                   (((imgbytes[i + 4]&2) << 2) & 0x08) | \
                   (((imgbytes[i + 5]&2) << 1) & 0x04) | \
                   (((imgbytes[i + 6]&2) << 0) & 0x02) | \
                   (((imgbytes[i + 7]&2) >> 1) & 0x01)

            imgnewbytes1[pointer] = chr(temp & 0xFF)

            pointer += 1
            i += 8
        return (imgnewbytes1, imgnewbytes0)

    def __convert(self, img, dither=True):
        (img_width, img_height) = img.size
        assert img_width == 1440
        assert img_height == 640*4

        img=img.convert('P')
        img=img.transpose(Image.FLIP_LEFT_RIGHT)

        pal = []
        for i in range(63):
                pal.extend([0,0,0])
        for i in range(64):
                pal.extend([85,85,85])
        for i in range(64):
                pal.extend([170,170,170])
        for i in range(65):
                pal.extend([255,255,255])
        assert len(pal) == 768

        if dither:
                img = img.convert('P', None, Image.FLOYDSTEINBERG, Image.WEB, 4)
                print "dithering!"
        img.save("img.png")
        img = img.convert('L')
        img.putpalette(pal)
        img = img.convert('L')
        
        imgbytes = list(img.tobytes())
        imgnewbytes=list(len(imgbytes)*b'\x00')

        trans1=( # new, original
            (6,7),
            (4,6),
            (2,5),
            (0,4),
            (14,3),
            (12,2),
            (10,1),
            (8,0),
            (7,15),
            (5,14),
            (3,13),
            (1,12),
            (15,11),
            (13,10),
            (11,9),
            (9,8)
           )

        for i in range(len(imgbytes)/16):
            for t,f in trans1:
                imgnewbytes[16*(i)+t]=imgbytes[16*(i)+f]

        imgnew = Image.frombytes('P',img.size,''.join(imgnewbytes))

        imgbytes = list(imgnew.tobytes())

        palette = {0:3, 85:2, 170:1, 255:0} #dictionary
        i = 0
        for byte in imgbytes:
               imgbytes[i] = palette[ord(byte)]
               i+=1
               
        (imgnewbytes1, imgnewbytes0) = self.rawImg2binData(imgbytes)

        xBytes = imgnew.size[0] / 8
        newstr = []
        for y in range(imgnew.size[1]):
            newstr.extend(imgnewbytes1[y*xBytes:(y+1)*xBytes])
            newstr.extend(imgnewbytes0[y*xBytes:(y+1)*xBytes]) 

        return "".join(newstr)

    def load_image_from_string(self, str):
        # save string as IO
        src = cStringIO.StringIO()
        src.write(str)
        src.seek(0)

        img = Image.open(src)
        self.imagedata = self.__convert(img)

    def load_image(self, src):
        # Open file
        img = Image.open(src)

        self.imagedata = self.__convert(img)

    def load_image_from_pil(self, obj):
        self.imagedata = self.__convert(obj)

"""
Convert image to EPD file.
Command line usage: TCM2-E312_2bit.py -i <inputfile> 
"""
def main(argv):
    inputfile = ''
    outputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print 'TCM2-E312_2bit.py -i <inputfile>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'TCM2-E312_2bit.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg

    if not inputfile:
        print 'usage: TCM2-E312_2bit.py -i <inputfile>'
        sys.exit(2)

    print 'Input file is ', inputfile

    if not os.path.isfile(inputfile):
        print "Input filename is not pointing to an existing location!"
        sys.exit(2)

    tcm = TCM2312()
    (head_fname, tail_fname) = os.path.split(inputfile)
    tail_fname = tail_fname.split('.')[0]
    outputfile = os.path.join(head_fname, tcm.filename_prefix
                              +tail_fname+tcm.filename_sufix+".epd")
    
    print 'Output file is ', outputfile, ' + .dlz'
    
    tcm.load_image(inputfile)

    fil = open(outputfile, 'w+b')
    fil.seek(0)
    fil.write(tcm.get_epd_header())
    fil.write(tcm.imagedata)
    fil.close()

    os.system("bcl_tcm.exe c lz "+outputfile+" "+outputfile[:-4]+".dlz")
    #now remove 12bytes long BCL header
    f = open(outputfile[:-4]+".dlz", 'rb')
    compressed = f.read()
    f.close()
    f = open(outputfile[:-4]+".dlz", 'wb')
    f.write(compressed[12:])
    f.close()

if __name__ == "__main__":
   main(sys.argv[1:])
