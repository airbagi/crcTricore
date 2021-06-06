# Tricore Aurix SAK-TC2xxxx CPU CRC Calc Utility

Simple utility to check whether block checksum is ok.
usage : 
`crcTricore <file.bin>`

Put the entire block to a binary file. Then run utility.
CRC32 is located at the end of each block. If the CRC is ok, utility writes "GOOD", otherwise it writes "BAD".
