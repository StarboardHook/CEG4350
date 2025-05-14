; File Allocation Table (First Copy)
fatCopy0:
times 2                 dw 0
kernelStartingCluster0  dw 3
                        dw 4
                        dw 5
                        dw 6
                        dw 7
                        dw 8
                        dw 9
                        dw 10
                        dw 11
                        dw 12
                        dw 13
                        dw 14
                        dw 15
                        dw 16
                        dw 17
                        dw 18
                        dw 19
                        dw 20
                        dw 21
                        dw 22
                        dw 23
                        dw 24
                        dw 25
                        dw 26
                        dw 27
                        dw 28
                        dw 29
                        dw 30
                        dw 31
                        dw 32
                        dw 33
                        dw 34
                        dw 35
                        dw 36
                        dw 37
                        dw 38
                        dw 39
                        dw 40
                        dw 41
                        dw 42
                        dw 43
                        dw 44
                        dw 45
                        dw 46
                        dw 47
                        dw 48
                        dw 49
                        dw 50
                        dw 0xFFFF
times (512 * 9) - ($ - fatCopy0) db 0

; NOTE: Make sure fatCopy0 and fatCopy1 have identical contents!

; File Allocation Table (Second Copy)
fatCopy1:
times 2                 dw 0
kernelStartingCluster1  dw 3
                        dw 4
                        dw 5
                        dw 6
                        dw 7
                        dw 8
                        dw 9
                        dw 10
                        dw 11
                        dw 12
                        dw 13
                        dw 14
                        dw 15
                        dw 16
                        dw 17
                        dw 18
                        dw 19
                        dw 20
                        dw 21
                        dw 22
                        dw 23
                        dw 24
                        dw 25
                        dw 26
                        dw 27
                        dw 28
                        dw 29
                        dw 30
                        dw 31
                        dw 32
                        dw 33
                        dw 34
                        dw 35
                        dw 36
                        dw 37
                        dw 38
                        dw 39
                        dw 40
                        dw 41
                        dw 42
                        dw 43
                        dw 44
                        dw 45
                        dw 46
                        dw 47
                        dw 48
                        dw 49
                        dw 50
                        dw 0xFFFF
times (512 * 9) - ($ - fatCopy1) db 0