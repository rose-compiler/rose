BITS 64
        ;; This file tests RegisterStateGeneric, which stores register values
        ;; during instruction semantics phases.  RegisterStateGeneric associates
        ;; a value with each location, but the set of locations can change
        ;; depending on which registers are accessed.  For instance, if the
        ;; AX location is stored while reading AL, then RegisterStateGeneric
        ;; can adjust its storage to have two locations: AH and AL.
        ;;
        ;; There are two settings for RegisterStateGeneric that control whether
        ;; it can modify its set of locations: whether it can modify locations
        ;; when a register is accessed (read or written), and whether it can
        ;; create new locations. We control these settings from within this
        ;; specimen when it is analyzed by the debugSemantics tool by setting
        ;; the stack pointer to a certain magic number.
        ;;
        ;; The following stack pointer values are magic:
        ;;   0x137017c0
        ;;   0x137017c1
        ;;   0x137017c2
        ;;   0x137017c3
        ;;
        ;; The least significant turns on or off the ability for the register
        ;; state to adjust its list of existing storage locations when reading
        ;; and writing, while the next more significant bit turns on or off
        ;; the ability for the register state to create new locations.  When a
        ;; bit is set the action is allowed; when clear, it's disable. When
        ;; the stack pointer doesn't contain a magic number it is treated as if
        ;; is contained 0x137017c3.

_start: hlt                                             ; static analysis

        ;; ----------------------------------------------------------------------
        ;; This section tests register reading with the default mode where reading
        ;; is allowed to change storage locations by combining and splitting
        ;; storage for the parts of registers that are accessed, thus adapting the
        ;; register state to contain those registers that are being accessed.
        push rbp
        mov rbp, rsp
        mov rax, 0x0706050403020100

        ;; successively smaller reads off the front break
        ;; the storage into smaller and smaller locations
        mov [0], rax                                    ; {0-63}
        mov [8], eax                                    ; {0-31, 32-63}
        mov [12], ax                                    ; {0-15, 16-31, 32-63}
        mov [14], al                                    ; {0-7, 8-15, 16-31, 32-63}

        ;; successively larger reads off the front will
        ;; rebuild the storage into larger and larger locations
        mov [15], ax                                    ; {0-15, 16-31, 32-63}
        mov [17], eax                                   ; {0-31, 32-63}
        mov [21], rax                                   ; {0-63}

        ;; Reading from the middle will split into three
        mov [29], ah                                    ; {0-7, 8-15, 16-63}

        leave
        ret

        ;; ----------------------------------------------------------------------
        ;; This section tests register reading when the register state is not
        ;; allowed to modify the storage layout.  We enable/disable this by
        ;; storing a magic number in the stack pointer, which is picked up by
        ;; the debugSemantics tool when run with "--test-adaptive-registers".
        push rbp
        mov rbp, rsp
        mov rax, 0x0706050403020100
        mov rsp, 0x137017c2                             ; turn off adaptive locations

        ;; successively smaller reads off the front will not break the storage
        ;; into smaller locations
        mov [0], rax                                    ; {0-63}
        mov [8], eax                                    ; {0-63}
        mov [12], ax                                    ; {0-63}
        mov [14], al                                    ; {0-63}

        leave
        ret

        ;; ----------------------------------------------------------------------
        ;; This sections tests that write operations properly adjust the
        ;; set of register storage locations.
        push rbp
        mov rbp, rsp

        ;; Split leading part into smaller and smaller locations
        mov rax, 0x0706050403020100                     ; {0-63}
        mov eax, 0xf3f2f1f0                             ; {0-31, 32-63 (=0)}
        mov ax,  0xe1e0                                 ; {0-15, 16-31, 32-63}
        mov al,  0xd0                                   ; {0-7, 8-15, 16-31, 32-63}

        ;; Join parts back together
        mov ax,  0xc1c0                                 ; {0-15, 16-31, 32-63}
        mov eax, 0xb3b2b1b0                             ; {0-31, 32-63}
        mov rax, 0xa7a6a5a4a3a2a1a0                     ; {0-63}

        ;; Writing to the middle splits three ways
        mov ah,  0xff
        
        leave
        ret

        ;; ----------------------------------------------------------------------
        ;; This sections tests that write operations do not adjust the set of
        ;; register storage locations when the stack pointer has a magic number
        ;; that disables this feature and debugSemantics has "--test-adaptive-registers"
        push rbp
        mov rbp, rsp
        mov rsp, 0x137017c2                             ; turn off adaptive locations

        ;; Split leading part into smaller and smaller locations
        mov rax, 0x0706050403020100                     ; {0-63}
        mov eax, 0xf3f2f1f0                             ; {0-31, 32-63 (=0)}
        mov ax,  0xe1e0                                 ; {0-15, 16-31, 32-63}
        mov al,  0xd0                                   ; {0-7, 8-15, 16-31, 32-63}

        ;; Join parts back together
        mov ax,  0xc1c0                                 ; {0-15, 16-31, 32-63}
        mov eax, 0xb3b2b1b0                             ; {0-31, 32-63}
        mov rax, 0xa7a6a5a4a3a2a1a0                     ; {0-63}

        ;; Writing to the middle splits three ways
        mov ah,  0xff
        
        leave
        ret

        ;; ----------------------------------------------------------------------
        push rbp
        mov rbp, rsp

        mov rax, 0x0706050403020100                     ; {0-63}

        mov rsp, 0x137017c1                             ; turn off location creation

        mov [0], eax                                    ; {0-31, 32-63}
        mov [4], rax                                    ; {0-63}

;       mov [12], ebx                                   ; FAILS to create ebx

        leave
        ret
        