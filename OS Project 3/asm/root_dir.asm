; Root Directory Contents
rootDir:
fileName            db "kernel  "
extension           db "bin"   
attributes          db 0
reserved            dw 0
creationTime        dw 0
creationDate        dw 0
lastAccessDate      dw 0
ignored             dw 0
lastWriteTime       dw 0
lastWriteDate       dw 0
startingCluster     dw 2
fileSize            dd 24576
times (512 * 14) - ($ - rootDir) db 0