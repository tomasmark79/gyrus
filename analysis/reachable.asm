; Reachable static control flow. Indirect dispatch and interrupt roots may be missing.
8028: 78        SEI
8029: a2 00     LDX #$00
802B: 8e 0e d4  STX $d40e
802E: 8a        TXA
802F: 95 00     STA $00,X
8031: e8        INX
8032: d0 fb     BNE $802f
8034: 20 65 8f  JSR $8f65
8037: a2 5f     LDX #$5f
8039: a9 9c     LDA #$9c
803B: 9d 00 20  STA $2000,X
803E: a9 3f     LDA #$3f
8040: 9d 60 20  STA $2060,X
8043: bd 00 20  LDA $2000,X
8046: 38        SEC
8047: e9 28     SBC #$28
8049: 9d ff 1f  STA $1fff,X
804C: bd 60 20  LDA $2060,X
804F: e9 00     SBC #$00
8051: 9d 5f 20  STA $205f,X
8054: ca        DEX
8055: d0 ec     BNE $8043
8057: 20 c7 a5  JSR $a5c7
805A: 20 99 a6  JSR $a699
805D: 20 68 a8  JSR $a868
8060: 20 5b 9d  JSR $9d5b
8063: 20 b8 87  JSR $87b8
8066: 20 f0 8e  JSR $8ef0
8069: a5 55     LDA $55
806B: c5 55     CMP $55
806D: f0 fc     BEQ $806b
806F: a5 57     LDA $57
8071: f0 06     BEQ $8079
8073: a5 93     LDA $93
8075: 05 ac     ORA $ac
8077: f0 03     BEQ $807c
8079: 4c b0 83  JMP $83b0
807C: a5 9a     LDA $9a
807E: f0 15     BEQ $8095
8080: c6 9a     DEC $9a
8082: d0 11     BNE $8095
8084: a5 fa     LDA $fa
8086: c9 3c     CMP #$3c
8088: f0 0b     BEQ $8095
808A: 20 6a 88  JSR $886a
808D: a2 00     LDX #$00
808F: 20 e3 8d  JSR $8de3
8092: 20 6e 88  JSR $886e
8095: a5 99     LDA $99
8097: 05 9a     ORA $9a
8099: 05 ac     ORA $ac
809B: d0 3d     BNE $80da
809D: a5 a0     LDA $a0
809F: c9 04     CMP #$04
80A1: f0 05     BEQ $80a8
80A3: b0 0f     BCS $80b4
80A5: 4c 37 81  JMP $8137
80A8: a5 d9     LDA $d9
80AA: f0 06     BEQ $80b2
80AC: a5 96     LDA $96
80AE: c9 06     CMP #$06
80B0: 90 f3     BCC $80a5
80B2: e6 a0     INC $a0
80B4: a9 03     LDA #$03
80B6: 85 f9     STA $f9
80B8: 85 e6     STA $e6
80BA: a5 af     LDA $af
80BC: d0 09     BNE $80c7
80BE: ad 0a d2  LDA $d20a
80C1: 29 3f     AND #$3f
80C3: 09 10     ORA #$10
80C5: 85 af     STA $af
80C7: a5 96     LDA $96
80C9: 05 a9     ORA $a9
80CB: 05 b1     ORA $b1
80CD: 05 b7     ORA $b7
80CF: 0d df 05  ORA $05df
80D2: 0d e0 05  ORA $05e0
80D5: 0d e1 05  ORA $05e1
80D8: 05 db     ORA $db
80DA: d0 5e     BNE $813a
80DC: a6 92     LDX $92
80DE: b5 ad     LDA $ad,X
80E0: f0 58     BEQ $813a
80E2: a5 b8     LDA $b8
80E4: 30 45     BMI $812b
80E6: 05 9a     ORA $9a
80E8: d0 50     BNE $813a
80EA: a5 d9     LDA $d9
80EC: d0 0c     BNE $80fa
80EE: 24 da     BIT $da
80F0: 70 08     BVS $80fa
80F2: 26 da     ROL $da
80F4: 38        SEC
80F5: 66 da     ROR $da
80F7: 4c 1f 83  JMP $831f
80FA: ad 76 05  LDA $0576
80FD: 85 b8     STA $b8
80FF: 20 a6 84  JSR $84a6
8102: a9 00     LDA #$00
8104: 8d 03 d0  STA $d003
8107: a6 92     LDX $92
8109: b5 9b     LDA $9b,X
810B: c9 01     CMP #$01
810D: d0 16     BNE $8125
810F: 85 bc     STA $bc
8111: 20 6a 88  JSR $886a
8114: a2 3c     LDX #$3c
8116: 20 de 98  JSR $98de
8119: 20 13 8c  JSR $8c13
811C: 20 38 9f  JSR $9f38
811F: 20 6e 88  JSR $886e
8122: 4c 3a 81  JMP $813a
8125: a9 06     LDA #$06
8127: 85 bd     STA $bd
8129: d0 0f     BNE $813a
812B: 20 6a 88  JSR $886a
812E: 20 25 94  JSR $9425
8131: 20 6e 88  JSR $886e
8134: 4c 3a 81  JMP $813a
8137: 20 34 96  JSR $9634
813A: a2 01     LDX #$01
813C: bd ce 05  LDA $05ce,X
813F: f0 02     BEQ $8143
8141: 10 03     BPL $8146
8143: 4c 17 82  JMP $8217
8146: a0 25     LDY #$25
8148: b9 ce 05  LDA $05ce,Y
814B: f0 1b     BEQ $8168
814D: 30 19     BMI $8168
814F: 29 7f     AND #$7f
8151: 85 59     STA $59
8153: 4a        LSR A
8154: 85 5a     STA $5a
8156: bd 6e 04  LDA $046e,X
8159: 18        CLC
815A: 65 5a     ADC $5a
815C: d9 6e 04  CMP $046e,Y
815F: 90 07     BCC $8168
8161: e5 59     SBC $59
8163: d9 6e 04  CMP $046e,Y
8166: 90 03     BCC $816b
8168: 4c 0f 82  JMP $820f
816B: bd c6 04  LDA $04c6,X
816E: 18        CLC
816F: 65 5a     ADC $5a
8171: d9 c6 04  CMP $04c6,Y
8174: 90 f2     BCC $8168
8176: e5 59     SBC $59
8178: d9 c6 04  CMP $04c6,Y
817B: b0 eb     BCS $8168
817D: 20 6a 88  JSR $886a
8180: b9 ce 05  LDA $05ce,Y
8183: f0 02     BEQ $8187
8185: 10 03     BPL $818a
8187: 4c 09 82  JMP $8209
818A: 09 80     ORA #$80
818C: 99 ce 05  STA $05ce,Y
818F: bd ce 05  LDA $05ce,X
8192: f0 1f     BEQ $81b3
8194: 09 80     ORA #$80
8196: 9d ce 05  STA $05ce,X
8199: e0 04     CPX #$04
819B: 90 0c     BCC $81a9
819D: bd cb 05  LDA $05cb,X
81A0: f0 11     BEQ $81b3
81A2: 09 80     ORA #$80
81A4: 9d cb 05  STA $05cb,X
81A7: 30 0a     BMI $81b3
81A9: bd d1 05  LDA $05d1,X
81AC: f0 05     BEQ $81b3
81AE: 09 80     ORA #$80
81B0: 9d d1 05  STA $05d1,X
81B3: b9 7e 06  LDA $067e,Y
81B6: 48        PHA
81B7: b9 1e 05  LDA $051e,Y
81BA: 48        PHA
81BB: b9 76 05  LDA $0576,Y
81BE: 48        PHA
81BF: 20 6e 88  JSR $886e
81C2: a0 0c     LDY #$0c
81C4: b9 ce 05  LDA $05ce,Y
81C7: f0 0b     BEQ $81d4
81C9: c8        INY
81CA: c0 11     CPY #$11
81CC: d0 f6     BNE $81c4
81CE: 68        PLA
81CF: 68        PLA
81D0: 68        PLA
81D1: 4c 17 82  JMP $8217
81D4: 68        PLA
81D5: 99 76 05  STA $0576,Y
81D8: 68        PLA
81D9: 99 1e 05  STA $051e,Y
81DC: 8a        TXA
81DD: 99 7e 06  STA $067e,Y
81E0: 68        PLA
81E1: 4a        LSR A
81E2: e9 02     SBC #$02
81E4: 90 04     BCC $81ea
81E6: c9 0b     CMP #$0b
81E8: 90 02     BCC $81ec
81EA: a9 04     LDA #$04
81EC: aa        TAX
81ED: bd eb b8  LDA $b8eb,X
81F0: 99 04 00  STA $0004,Y
81F3: bd f6 b8  LDA $b8f6,X
81F6: 99 09 00  STA $0009,Y
81F9: b9 7e 06  LDA $067e,Y
81FC: aa        TAX
81FD: a9 1c     LDA #$1c
81FF: 99 7e 06  STA $067e,Y
8202: a9 80     LDA #$80
8204: 99 ce 05  STA $05ce,Y
8207: d0 0e     BNE $8217
8209: 20 6e 88  JSR $886e
820C: 4c 17 82  JMP $8217
820F: c8        INY
8210: c0 58     CPY #$58
8212: f0 03     BEQ $8217
8214: 4c 48 81  JMP $8148
8217: e8        INX
8218: e0 07     CPX #$07
821A: f0 03     BEQ $821f
821C: 4c 3c 81  JMP $813c
821F: a5 a9     LDA $a9
8221: c5 a6     CMP $a6
8223: b0 14     BCS $8239
8225: a5 b3     LDA $b3
8227: f0 0a     BEQ $8233
8229: a2 00     LDX #$00
822B: 86 b3     STX $b3
822D: 18        CLC
822E: 69 30     ADC #$30
8230: aa        TAX
8231: d0 3d     BNE $8270
8233: a5 a7     LDA $a7
8235: d0 70     BNE $82a7
8237: a5 d9     LDA $d9
8239: f0 6c     BEQ $82a7
823B: a2 27     LDX #$27
823D: bd ce 05  LDA $05ce,X
8240: f0 20     BEQ $8262
8242: 30 1e     BMI $8262
8244: bd 76 05  LDA $0576,X
8247: c9 1a     CMP #$1a
8249: 90 17     BCC $8262
824B: bd 1e 05  LDA $051e,X
824E: c5 b2     CMP $b2
8250: f0 17     BEQ $8269
8252: e9 01     SBC #$01
8254: 29 7e     AND #$7e
8256: c5 b2     CMP $b2
8258: f0 0f     BEQ $8269
825A: 69 04     ADC #$04
825C: 29 7e     AND #$7e
825E: c5 b2     CMP $b2
8260: f0 07     BEQ $8269
8262: e8        INX
8263: e0 58     CPX #$58
8265: d0 d6     BNE $823d
8267: f0 3e     BEQ $82a7
8269: ad 0a d2  LDA $d20a
826C: 29 03     AND #$03
826E: d0 37     BNE $82a7
8270: a0 07     LDY #$07
8272: b9 ce 05  LDA $05ce,Y
8275: f0 03     BEQ $827a
8277: c8        INY
8278: d0 f8     BNE $8272
827A: ad 0a d2  LDA $d20a
827D: 29 06     AND #$06
827F: 38        SEC
8280: e9 02     SBC #$02
8282: c9 04     CMP #$04
8284: d0 02     BNE $8288
8286: a9 00     LDA #$00
8288: 18        CLC
8289: 7d 1e 05  ADC $051e,X
828C: 29 7e     AND #$7e
828E: 99 1e 05  STA $051e,Y
8291: bd 76 05  LDA $0576,X
8294: 99 76 05  STA $0576,Y
8297: a9 26     LDA #$26
8299: 99 7e 06  STA $067e,Y
829C: a9 80     LDA #$80
829E: 99 ce 05  STA $05ce,Y
82A1: e6 a9     INC $a9
82A3: a5 a8     LDA $a8
82A5: 85 a7     STA $a7
82A7: a6 92     LDX $92
82A9: b5 ad     LDA $ad,X
82AB: d0 10     BNE $82bd
82AD: a5 e6     LDA $e6
82AF: f0 0c     BEQ $82bd
82B1: a5 96     LDA $96
82B3: c9 03     CMP #$03
82B5: b0 09     BCS $82c0
82B7: a9 00     LDA #$00
82B9: 85 b1     STA $b1
82BB: d6 ad     DEC $ad,X
82BD: 4c 1f 83  JMP $831f
82C0: c9 06     CMP #$06
82C2: 90 0a     BCC $82ce
82C4: a5 55     LDA $55
82C6: 29 0c     AND #$0c
82C8: d0 55     BNE $831f
82CA: c6 af     DEC $af
82CC: d0 51     BNE $831f
82CE: a9 04     LDA #$04
82D0: 8d af 06  STA $06af
82D3: 8d b1 06  STA $06b1
82D6: a6 92     LDX $92
82D8: b4 94     LDY $94,X
82DA: d0 02     BNE $82de
82DC: a9 10     LDA #$10
82DE: 8d b0 06  STA $06b0
82E1: a0 06     LDY #$06
82E3: ad 1e 05  LDA $051e
82E6: 18        CLC
82E7: 79 7c ac  ADC $ac7c,Y
82EA: 29 7e     AND #$7e
82EC: 99 34 05  STA $0534,Y
82EF: b9 82 ac  LDA $ac82,Y
82F2: 99 8c 05  STA $058c,Y
82F5: a9 00     LDA #$00
82F7: 99 3c 06  STA $063c,Y
82FA: 88        DEY
82FB: d0 e6     BNE $82e3
82FD: a9 8c     LDA #$8c
82FF: 85 db     STA $db
8301: ad 1e 05  LDA $051e
8304: 38        SEC
8305: e9 0c     SBC #$0c
8307: a2 03     LDX #$03
8309: 29 7e     AND #$7e
830B: 9d 4e 05  STA $054e,X
830E: 48        PHA
830F: a9 32     LDA #$32
8311: 9d a6 05  STA $05a6,X
8314: 68        PLA
8315: 18        CLC
8316: 69 0c     ADC #$0c
8318: ca        DEX
8319: d0 ee     BNE $8309
831B: a6 92     LDX $92
831D: f6 ad     INC $ad,X
831F: a5 b4     LDA $b4
8321: d0 0b     BNE $832e
8323: ad d9 05  LDA $05d9
8326: d0 2b     BNE $8353
8328: a5 f6     LDA $f6
832A: 85 b4     STA $b4
832C: f0 25     BEQ $8353
832E: c6 b4     DEC $b4
8330: d0 21     BNE $8353
8332: a5 b8     LDA $b8
8334: d0 1d     BNE $8353
8336: a5 d9     LDA $d9
8338: f0 19     BEQ $8353
833A: a9 18     LDA #$18
833C: 8d 89 06  STA $0689
833F: a9 32     LDA #$32
8341: 8d 81 05  STA $0581
8344: a9 02     LDA #$02
8346: 4d 1e 05  EOR $051e
8349: 8d 29 05  STA $0529
834C: e6 a9     INC $a9
834E: a9 80     LDA #$80
8350: 8d d9 05  STA $05d9
8353: a5 b6     LDA $b6
8355: f0 04     BEQ $835b
8357: c6 b6     DEC $b6
8359: d0 55     BNE $83b0
835B: a5 b7     LDA $b7
835D: 05 9a     ORA $9a
835F: 05 b8     ORA $b8
8361: d0 4d     BNE $83b0
8363: a5 d9     LDA $d9
8365: f0 49     BEQ $83b0
8367: a6 92     LDX $92
8369: b5 a3     LDA $a3,X
836B: c9 05     CMP #$05
836D: 90 41     BCC $83b0
836F: ad 0a d2  LDA $d20a
8372: d0 3c     BNE $83b0
8374: ad 0a d2  LDA $d20a
8377: 29 07     AND #$07
8379: d0 35     BNE $83b0
837B: ad 0a d2  LDA $d20a
837E: 29 70     AND #$70
8380: a2 02     LDX #$02
8382: 9d 42 05  STA $0542,X
8385: 69 0e     ADC #$0e
8387: 29 7e     AND #$7e
8389: 48        PHA
838A: a9 32     LDA #$32
838C: 9d 9a 05  STA $059a,X
838F: 68        PLA
8390: ca        DEX
8391: d0 ef     BNE $8382
8393: 8d 42 05  STA $0542
8396: a9 32     LDA #$32
8398: 8d 9a 05  STA $059a
839B: a9 14     LDA #$14
839D: 8d a2 06  STA $06a2
83A0: a9 12     LDA #$12
83A2: 8d a3 06  STA $06a3
83A5: a9 1a     LDA #$1a
83A7: 8d a4 06  STA $06a4
83AA: a9 02     LDA #$02
83AC: 85 b5     STA $b5
83AE: c6 b7     DEC $b7
83B0: a5 8e     LDA $8e
83B2: f0 0b     BEQ $83bf
83B4: a2 00     LDX #$00
83B6: 86 8e     STX $8e
83B8: 86 57     STX $57
83BA: e8        INX
83BB: 86 91     STX $91
83BD: d0 08     BNE $83c7
83BF: a5 8d     LDA $8d
83C1: f0 3f     BEQ $8402
83C3: a9 00     LDA #$00
83C5: 85 91     STA $91
83C7: 20 79 8c  JSR $8c79
83CA: a9 05     LDA #$05
83CC: 85 f7     STA $f7
83CE: 85 f8     STA $f8
83D0: 20 62 8e  JSR $8e62
83D3: a2 00     LDX #$00
83D5: 86 8d     STX $8d
83D7: 86 ab     STX $ab
83D9: a2 05     LDX #$05
83DB: 86 57     STX $57
83DD: 86 aa     STX $aa
83DF: a5 91     LDA $91
83E1: f0 02     BEQ $83e5
83E3: 86 ab     STX $ab
83E5: a2 02     LDX #$02
83E7: 86 9b     STX $9b
83E9: 86 9c     STX $9c
83EB: ca        DEX
83EC: 86 a3     STX $a3
83EE: 86 a4     STX $a4
83F0: ca        DEX
83F1: 86 92     STX $92
83F3: 86 a1     STX $a1
83F5: 86 a2     STX $a2
83F7: 20 e6 92  JSR $92e6
83FA: a9 00     LDA #$00
83FC: 20 01 b9  JSR $b901
83FF: 20 85 8c  JSR $8c85
8402: 4c 69 80  JMP $8069
8405: 2c 0b d4  BIT $d40b
8408: 10 fb     BPL $8405
840A: 60        RTS
840B: a5 e6     LDA $e6
840D: f0 fb     BEQ $840a
840F: ad 0a d2  LDA $d20a
8412: 25 e9     AND $e9
8414: 05 ac     ORA $ac
8416: d0 f2     BNE $840a
8418: a5 99     LDA $99
841A: c5 e8     CMP $e8
841C: f0 ec     BEQ $840a
841E: a2 34     LDX #$34
8420: bd ce 05  LDA $05ce,X
8423: f0 02     BEQ $8427
8425: 10 06     BPL $842d
8427: e8        INX
8428: e0 58     CPX #$58
842A: d0 f4     BNE $8420
842C: 60        RTS
842D: 20 af 87  JSR $87af
8430: 20 9a 9a  JSR $9a9a
8433: a9 00     LDA #$00
8435: 9d ce 05  STA $05ce,X
8438: c6 96     DEC $96
843A: e6 99     INC $99
843C: 86 4e     STX $4e
843E: a2 00     LDX #$00
8440: bd f5 05  LDA $05f5,X
8443: f0 03     BEQ $8448
8445: e8        INX
8446: d0 f8     BNE $8440
8448: a5 4e     LDA $4e
844A: 38        SEC
844B: e9 33     SBC #$33
844D: 95 2e     STA $2e,X
844F: a8        TAY
8450: b9 51 05  LDA $0551,Y
8453: 9d 45 05  STA $0545,X
8456: a9 26     LDA #$26
8458: 9d 9d 05  STA $059d,X
845B: 20 40 92  JSR $9240
845E: 20 74 92  JSR $9274
8461: a9 ff     LDA #$ff
8463: 95 24     STA $24,X
8465: b9 d6 08  LDA $08d6,Y
8468: 29 0f     AND #$0f
846A: 9d a5 06  STA $06a5,X
846D: b9 d6 08  LDA $08d6,Y
8470: 4a        LSR A
8471: 4a        LSR A
8472: 4a        LSR A
8473: 4a        LSR A
8474: 9d 4d 06  STA $064d,X
8477: 8a        TXA
8478: 18        CLC
8479: 69 27     ADC #$27
847B: aa        TAX
847C: 20 97 9a  JSR $9a97
847F: a5 99     LDA $99
8481: 4a        LSR A
8482: b0 94     BCS $8418
8484: 60        RTS
8485: a2 08     LDX #$08
8487: 4c 65 8f  JMP $8f65
848A: a2 04     LDX #$04
848C: 20 65 8f  JSR $8f65
848F: a2 14     LDX #$14
8491: a9 ff     LDA #$ff
8493: 85 a0     STA $a0
8495: 9d e7 20  STA $20e7,X
8498: ca        DEX
8499: d0 fa     BNE $8495
849B: a2 58     LDX #$58
849D: a9 00     LDA #$00
849F: 9d cd 05  STA $05cd,X
84A2: ca        DEX
84A3: d0 fa     BNE $849f
84A5: 60        RTS
84A6: a2 0c     LDX #$0c
84A8: 4c 65 8f  JMP $8f65
84AB: 48        PHA
84AC: 8a        TXA
84AD: 48        PHA
84AE: 98        TYA
84AF: 48        PHA
84B0: 20 6a 88  JSR $886a
84B3: a5 da     LDA $da
84B5: 10 36     BPL $84ed
84B7: 0a        ASL A
84B8: f0 16     BEQ $84d0
84BA: 4a        LSR A
84BB: aa        TAX
84BC: e0 28     CPX #$28
84BE: f0 08     BEQ $84c8
84C0: a9 02     LDA #$02
84C2: 20 2f 98  JSR $982f
84C5: e8        INX
84C6: d0 f4     BNE $84bc
84C8: a2 50     LDX #$50
84CA: 20 de 98  JSR $98de
84CD: 4c e4 84  JMP $84e4
84D0: a2 0a     LDX #$0a
84D2: a9 14     LDA #$14
84D4: 20 2f 98  JSR $982f
84D7: ca        DEX
84D8: d0 f8     BNE $84d2
84DA: a9 02     LDA #$02
84DC: 20 01 b9  JSR $b901
84DF: a2 1b     LDX #$1b
84E1: 20 e3 8d  JSR $8de3
84E4: 8d 1e d0  STA $d01e
84E7: 46 da     LSR $da
84E9: a9 ff     LDA #$ff
84EB: 85 9a     STA $9a
84ED: a4 bd     LDY $bd
84EF: f0 0c     BEQ $84fd
84F1: 20 16 bc  JSR $bc16
84F4: a4 bd     LDY $bd
84F6: 20 95 b9  JSR $b995
84F9: a9 00     LDA #$00
84FB: 85 bd     STA $bd
84FD: a5 bc     LDA $bc
84FF: f0 07     BEQ $8508
8501: 20 01 b9  JSR $b901
8504: a9 00     LDA #$00
8506: 85 bc     STA $bc
8508: a5 57     LDA $57
850A: d0 14     BNE $8520
850C: ad 0b d4  LDA $d40b
850F: c9 40     CMP #$40
8511: b0 08     BCS $851b
8513: a2 03     LDX #$03
8515: 20 c2 8d  JSR $8dc2
8518: 4c a0 86  JMP $86a0
851B: a2 00     LDX #$00
851D: 20 c2 8d  JSR $8dc2
8520: e6 55     INC $55
8522: a5 d8     LDA $d8
8524: f0 03     BEQ $8529
8526: 20 c4 8c  JSR $8cc4
8529: a5 a7     LDA $a7
852B: f0 02     BEQ $852f
852D: c6 a7     DEC $a7
852F: 20 21 9a  JSR $9a21
8532: a5 93     LDA $93
8534: f0 06     BEQ $853c
8536: 20 6e 8c  JSR $8c6e
8539: 4c a0 86  JMP $86a0
853C: 20 1e 8d  JSR $8d1e
853F: a5 57     LDA $57
8541: d0 03     BNE $8546
8543: 4c a0 86  JMP $86a0
8546: a5 9a     LDA $9a
8548: d0 0f     BNE $8559
854A: 20 13 89  JSR $8913
854D: 20 f8 89  JSR $89f8
8550: 20 76 88  JSR $8876
8553: 20 0c 87  JSR $870c
8556: 20 0b 84  JSR $840b
8559: 20 a9 86  JSR $86a9
855C: 20 21 8c  JSR $8c21
855F: a5 ac     LDA $ac
8561: f0 62     BEQ $85c5
8563: c6 ac     DEC $ac
8565: d0 06     BNE $856d
8567: a5 99     LDA $99
8569: f0 05     BEQ $8570
856B: 85 ac     STA $ac
856D: 4c fc 85  JMP $85fc
8570: a6 92     LDX $92
8572: a0 03     LDY #$03
8574: b5 ad     LDA $ad,X
8576: f0 0e     BEQ $8586
8578: b5 f7     LDA $f7,X
857A: f0 06     BEQ $8582
857C: a9 00     LDA #$00
857E: 95 f7     STA $f7,X
8580: f0 04     BEQ $8586
8582: a9 80     LDA #$80
8584: a0 00     LDY #$00
8586: 84 b1     STY $b1
8588: 95 ad     STA $ad,X
858A: a5 a0     LDA $a0
858C: 95 a1     STA $a1,X
858E: a5 96     LDA $96
8590: 18        CLC
8591: 65 99     ADC $99
8593: 95 97     STA $97,X
8595: b5 f7     LDA $f7,X
8597: f0 02     BEQ $859b
8599: d6 f7     DEC $f7,X
859B: d6 aa     DEC $aa,X
859D: 20 bc 98  JSR $98bc
85A0: a5 92     LDA $92
85A2: 49 01     EOR #$01
85A4: aa        TAX
85A5: b5 aa     LDA $aa,X
85A7: d0 0e     BNE $85b7
85A9: a6 92     LDX $92
85AB: b5 aa     LDA $aa,X
85AD: d0 08     BNE $85b7
85AF: 85 57     STA $57
85B1: 20 f0 8e  JSR $8ef0
85B4: 4c a0 86  JMP $86a0
85B7: 86 92     STX $92
85B9: a9 00     LDA #$00
85BB: 95 94     STA $94,X
85BD: c6 ac     DEC $ac
85BF: 20 e6 92  JSR $92e6
85C2: 4c a0 86  JMP $86a0
85C5: 20 6b 8b  JSR $8b6b
85C8: a5 b8     LDA $b8
85CA: d0 30     BNE $85fc
85CC: a5 d9     LDA $d9
85CE: f0 2c     BEQ $85fc
85D0: ad 04 d0  LDA $d004
85D3: 0d 05 d0  ORA $d005
85D6: 0d 06 d0  ORA $d006
85D9: 8d 1e d0  STA $d01e
85DC: 25 f9     AND $f9
85DE: f0 1c     BEQ $85fc
85E0: a0 00     LDY #$00
85E2: 20 95 b9  JSR $b995
85E5: a9 5a     LDA #$5a
85E7: 85 ac     STA $ac
85E9: a0 80     LDY #$80
85EB: 20 1d a0  JSR $a01d
85EE: a9 00     LDA #$00
85F0: 8d 00 d0  STA $d000
85F3: 8d 01 d0  STA $d001
85F6: 8d 02 d0  STA $d002
85F9: 8d 03 d0  STA $d003
85FC: 20 56 89  JSR $8956
85FF: a5 a5     LDA $a5
8601: f0 09     BEQ $860c
8603: c6 a5     DEC $a5
8605: d0 13     BNE $861a
8607: a0 02     LDY #$02
8609: 20 95 b9  JSR $b995
860C: c6 ef     DEC $ef
860E: d0 03     BNE $8613
8610: 20 96 8f  JSR $8f96
8613: c6 f1     DEC $f1
8615: d0 03     BNE $861a
8617: 20 8e 8f  JSR $8f8e
861A: a5 99     LDA $99
861C: d0 02     BNE $8620
861E: 85 9f     STA $9f
8620: a5 56     LDA $56
8622: e6 56     INC $56
8624: 29 0f     AND #$0f
8626: d0 9a     BNE $85c2
8628: e6 5b     INC $5b
862A: a5 5b     LDA $5b
862C: a2 24     LDX #$24
862E: 4a        LSR A
862F: 90 0f     BCC $8640
8631: bd 51 05  LDA $0551,X
8634: 69 02     ADC #$02
8636: 29 7e     AND #$7e
8638: 9d 51 05  STA $0551,X
863B: ca        DEX
863C: d0 f3     BNE $8631
863E: f0 0b     BEQ $864b
8640: bd a9 05  LDA $05a9,X
8643: 49 02     EOR #$02
8645: 9d a9 05  STA $05a9,X
8648: ca        DEX
8649: d0 f5     BNE $8640
864B: a2 34     LDX #$34
864D: bd ce 05  LDA $05ce,X
8650: f0 49     BEQ $869b
8652: bd 26 06  LDA $0626,X
8655: 0a        ASL A
8656: 0a        ASL A
8657: 85 4f     STA $4f
8659: bd 6e 04  LDA $046e,X
865C: 38        SEC
865D: e9 01     SBC #$01
865F: bc c6 04  LDY $04c6,X
8662: 88        DEY
8663: 20 8a 87  JSR $878a
8666: a5 4e     LDA $4e
8668: 0a        ASL A
8669: 0a        ASL A
866A: 0a        ASL A
866B: 0a        ASL A
866C: 05 4f     ORA $4f
866E: a8        TAY
866F: a9 8b     LDA #$8b
8671: 85 48     STA $48
8673: a9 ad     LDA #$ad
8675: 85 49     STA $49
8677: 86 58     STX $58
8679: a9 02     LDA #$02
867B: 85 4e     STA $4e
867D: 85 4f     STA $4f
867F: 20 9c 9b  JSR $9b9c
8682: bd ce 05  LDA $05ce,X
8685: 10 0e     BPL $8695
8687: c6 96     DEC $96
8689: a9 01     LDA #$01
868B: 20 2f 98  JSR $982f
868E: a9 00     LDA #$00
8690: 9d ce 05  STA $05ce,X
8693: f0 06     BEQ $869b
8695: 20 af 87  JSR $87af
8698: 20 9a 9a  JSR $9a9a
869B: e8        INX
869C: e0 58     CPX #$58
869E: d0 ad     BNE $864d
86A0: 68        PLA
86A1: a8        TAY
86A2: 68        PLA
86A3: aa        TAX
86A4: 20 6e 88  JSR $886e
86A7: 68        PLA
86A8: 40        RTI
86A9: a5 55     LDA $55
86AB: 29 03     AND #$03
86AD: c9 02     CMP #$02
86AF: d0 5a     BNE $870b
86B1: a2 0c     LDX #$0c
86B3: bd ce 05  LDA $05ce,X
86B6: f0 4e     BEQ $8706
86B8: 10 0a     BPL $86c4
86BA: a9 0a     LDA #$0a
86BC: 9d ce 05  STA $05ce,X
86BF: 9d ff ff  STA $ffff,X
86C2: d0 38     BNE $86fc
86C4: 20 f8 9a  JSR $9af8
86C7: bd ff ff  LDA $ffff,X
86CA: 29 03     AND #$03
86CC: c9 01     CMP #$01
86CE: d0 18     BNE $86e8
86D0: bd 76 05  LDA $0576,X
86D3: 18        CLC
86D4: 75 04     ADC $04,X
86D6: c9 14     CMP #$14
86D8: 90 03     BCC $86dd
86DA: 9d 76 05  STA $0576,X
86DD: bd 1e 05  LDA $051e,X
86E0: 18        CLC
86E1: 75 09     ADC $09,X
86E3: 29 7e     AND #$7e
86E5: 9d 1e 05  STA $051e,X
86E8: bd 7e 06  LDA $067e,X
86EB: 18        CLC
86EC: 69 02     ADC #$02
86EE: c9 22     CMP #$22
86F0: 90 02     BCC $86f4
86F2: a9 1c     LDA #$1c
86F4: 9d 7e 06  STA $067e,X
86F7: de ff ff  DEC $ffff,X
86FA: f0 05     BEQ $8701
86FC: 20 97 9a  JSR $9a97
86FF: 90 05     BCC $8706
8701: a9 00     LDA #$00
8703: 9d ce 05  STA $05ce,X
8706: e8        INX
8707: e0 11     CPX #$11
8709: d0 a8     BNE $86b3
870B: 60        RTS
870C: a9 03     LDA #$03
870E: 24 db     BIT $db
8710: 50 01     BVC $8713
8712: 4a        LSR A
8713: 25 55     AND $55
8715: d0 72     BNE $8789
8717: a5 db     LDA $db
8719: f0 6e     BEQ $8789
871B: a2 1c     LDX #$1c
871D: 20 5b 88  JSR $885b
8720: bd 76 05  LDA $0576,X
8723: f0 2a     BEQ $874f
8725: 24 db     BIT $db
8727: 30 08     BMI $8731
8729: 20 fb 9a  JSR $9afb
872C: a4 db     LDY $db
872E: 88        DEY
872F: f0 1e     BEQ $874f
8731: 20 9a 9a  JSR $9a9a
8734: bd 72 ac  LDA $ac72,X
8737: 24 db     BIT $db
8739: 50 01     BVC $873c
873B: 0a        ASL A
873C: 18        CLC
873D: 7d 1e 05  ADC $051e,X
8740: 29 7e     AND #$7e
8742: 9d 1e 05  STA $051e,X
8745: bd 76 05  LDA $0576,X
8748: 18        CLC
8749: 7d 78 ac  ADC $ac78,X
874C: 9d 76 05  STA $0576,X
874F: ca        DEX
8750: e0 16     CPX #$16
8752: d0 cc     BNE $8720
8754: a5 db     LDA $db
8756: 29 7f     AND #$7f
8758: 85 db     STA $db
875A: 0a        ASL A
875B: 10 2a     BPL $8787
875D: e8        INX
875E: bd 76 05  LDA $0576,X
8761: f0 1f     BEQ $8782
8763: bd 1e 05  LDA $051e,X
8766: 4d 1e 05  EOR $051e
8769: 29 f8     AND #$f8
876B: d0 15     BNE $8782
876D: 9d 76 05  STA $0576,X
8770: 20 fb 9a  JSR $9afb
8773: c6 db     DEC $db
8775: a5 db     LDA $db
8777: 29 03     AND #$03
8779: d0 07     BNE $8782
877B: a6 92     LDX $92
877D: f6 94     INC $94,X
877F: 85 db     STA $db
8781: 60        RTS
8782: e0 19     CPX #$19
8784: d0 d7     BNE $875d
8786: 60        RTS
8787: c6 db     DEC $db
8789: 60        RTS
878A: 48        PHA
878B: 29 03     AND #$03
878D: 85 4e     STA $4e
878F: 68        PLA
8790: 4a        LSR A
8791: 4a        LSR A
8792: 18        CLC
8793: 79 00 20  ADC $2000,Y
8796: 85 09     STA $09
8798: a9 00     LDA #$00
879A: 79 60 20  ADC $2060,Y
879D: 85 0a     STA $0a
879F: a0 00     LDY #$00
87A1: 60        RTS
87AF: a9 7c     LDA #$7c
87B1: 85 46     STA $46
87B3: a9 ad     LDA #$ad
87B5: 85 47     STA $47
87B7: 60        RTS
87B8: a9 94     LDA #$94
87BA: 8d 22 02  STA $0222
87BD: a9 9a     LDA #$9a
87BF: 8d 23 02  STA $0223
87C2: 20 79 8c  JSR $8c79
87C5: a2 00     LDX #$00
87C7: bd 00 e0  LDA $e000,X
87CA: 9d 00 1e  STA $1e00,X
87CD: bd 00 e1  LDA $e100,X
87D0: 9d 00 1f  STA $1f00,X
87D3: e8        INX
87D4: d0 f1     BNE $87c7
87D6: a2 08     LDX #$08
87D8: bd 1e 88  LDA $881e,X
87DB: 9d f8 1f  STA $1ff8,X
87DE: ca        DEX
87DF: d0 f7     BNE $87d8
87E1: a9 1e     LDA #$1e
87E3: 8d 09 d4  STA $d409
87E6: a2 04     LDX #$04
87E8: 8e 02 d3  STX $d302
87EB: ca        DEX
87EC: 8e 1d d0  STX $d01d
87EF: 8e 0f d2  STX $d20f
87F2: ca        DEX
87F3: 8e 0f d2  STX $d20f
87F6: a9 40     LDA #$40
87F8: 8d 0a d2  STA $d20a
87FB: 8d 0e d2  STA $d20e
87FE: a2 11     LDX #$11
8800: 8e 1b d0  STX $d01b
8803: a9 20     LDA #$20
8805: 8d 07 d4  STA $d407
8808: a9 00     LDA #$00
880A: 8d 02 d4  STA $d402
880D: a9 04     LDA #$04
880F: 8d 03 d4  STA $d403
8812: a9 ab     LDA #$ab
8814: 8d 00 02  STA $0200
8817: a9 84     LDA #$84
8819: 8d 01 02  STA $0201
881C: 4c 85 8c  JMP $8c85
8827: a2 00     LDX #$00
8829: bd 4e 88  LDA $884e,X
882C: 9d 00 04  STA $0400,X
882F: e8        INX
8830: e0 09     CPX #$09
8832: d0 f5     BNE $8829
8834: a0 5e     LDY #$5e
8836: a9 0d     LDA #$0d
8838: 9d 00 04  STA $0400,X
883B: e8        INX
883C: 88        DEY
883D: d0 f9     BNE $8838
883F: a0 00     LDY #$00
8841: b9 57 88  LDA $8857,Y
8844: 9d 00 04  STA $0400,X
8847: e8        INX
8848: c8        INY
8849: c0 04     CPY #$04
884B: d0 f4     BNE $8841
884D: 60        RTS
885B: bd 7e 06  LDA $067e,X
885E: a8        TAY
885F: b9 95 ac  LDA $ac95,Y
8862: 85 46     STA $46
8864: b9 96 ac  LDA $ac96,Y
8867: 85 47     STA $47
8869: 60        RTS
886A: a9 40     LDA #$40
886C: d0 02     BNE $8870
886E: a9 c0     LDA #$c0
8870: 85 cc     STA $cc
8872: 8d 0e d4  STA $d40e
8875: 60        RTS
8876: a5 55     LDA $55
8878: 29 0f     AND #$0f
887A: d0 48     BNE $88c4
887C: a2 26     LDX #$26
887E: a5 b7     LDA $b7
8880: 30 15     BMI $8897
8882: bd ce 05  LDA $05ce,X
8885: f0 38     BEQ $88bf
8887: 20 f8 9a  JSR $9af8
888A: bd ce 05  LDA $05ce,X
888D: 10 08     BPL $8897
888F: a9 02     LDA #$02
8891: 20 2f 98  JSR $982f
8894: 4c aa 88  JMP $88aa
8897: de 76 05  DEC $0576,X
889A: de 76 05  DEC $0576,X
889D: bd 76 05  LDA $0576,X
88A0: 30 08     BMI $88aa
88A2: 8d 9a 05  STA $059a
88A5: 20 97 9a  JSR $9a97
88A8: 90 15     BCC $88bf
88AA: c6 b7     DEC $b7
88AC: a9 b4     LDA #$b4
88AE: 85 b6     STA $b6
88B0: a9 00     LDA #$00
88B2: 9d ce 05  STA $05ce,X
88B5: ad f2 05  LDA $05f2
88B8: f0 05     BEQ $88bf
88BA: 09 80     ORA #$80
88BC: 8d f2 05  STA $05f2
88BF: ca        DEX
88C0: e0 24     CPX #$24
88C2: d0 ba     BNE $887e
88C4: a5 b7     LDA $b7
88C6: 10 0c     BPL $88d4
88C8: a5 55     LDA $55
88CA: 29 0f     AND #$0f
88CC: d0 42     BNE $8910
88CE: a9 03     LDA #$03
88D0: 85 b7     STA $b7
88D2: d0 11     BNE $88e5
88D4: ad f2 05  LDA $05f2
88D7: f0 37     BEQ $8910
88D9: a2 24     LDX #$24
88DB: 20 f8 9a  JSR $9af8
88DE: b0 25     BCS $8905
88E0: ad f2 05  LDA $05f2
88E3: 30 20     BMI $8905
88E5: a6 b5     LDX $b5
88E7: ad 42 05  LDA $0542
88EA: 18        CLC
88EB: 7d 10 89  ADC $8910,X
88EE: 29 7e     AND #$7e
88F0: 8d 42 05  STA $0542
88F3: dd 42 05  CMP $0542,X
88F6: d0 06     BNE $88fe
88F8: a5 b5     LDA $b5
88FA: 49 03     EOR #$03
88FC: 85 b5     STA $b5
88FE: a2 24     LDX #$24
8900: 20 97 9a  JSR $9a97
8903: 90 0b     BCC $8910
8905: c6 b7     DEC $b7
8907: a9 b4     LDA #$b4
8909: 85 b6     STA $b6
890B: a9 00     LDA #$00
890D: 8d f2 05  STA $05f2
8910: 60        RTS
8913: a5 56     LDA $56
8915: 29 03     AND #$03
8917: d0 3c     BNE $8955
8919: a2 07     LDX #$07
891B: bc ce 05  LDY $05ce,X
891E: f0 30     BEQ $8950
8920: 10 05     BPL $8927
8922: 5e ce 05  LSR $05ce,X
8925: d0 1d     BNE $8944
8927: 20 f8 9a  JSR $9af8
892A: a5 56     LDA $56
892C: 29 04     AND #$04
892E: f0 0c     BEQ $893c
8930: e0 0b     CPX #$0b
8932: f0 08     BEQ $893c
8934: bd 7e 06  LDA $067e,X
8937: 49 02     EOR #$02
8939: 9d 7e 06  STA $067e,X
893C: de 76 05  DEC $0576,X
893F: de 76 05  DEC $0576,X
8942: 30 05     BMI $8949
8944: 20 97 9a  JSR $9a97
8947: 90 07     BCC $8950
8949: c6 a9     DEC $a9
894B: a9 00     LDA #$00
894D: 9d ce 05  STA $05ce,X
8950: e8        INX
8951: e0 0c     CPX #$0c
8953: d0 c6     BNE $891b
8955: 60        RTS
8956: a9 e4     LDA #$e4
8958: 85 46     STA $46
895A: a9 b5     LDA #$b5
895C: 85 47     STA $47
895E: a2 01     LDX #$01
8960: bc ce 05  LDY $05ce,X
8963: f0 1f     BEQ $8984
8965: 20 fb 9a  JSR $9afb
8968: bd ce 05  LDA $05ce,X
896B: 30 0a     BMI $8977
896D: bd 76 05  LDA $0576,X
8970: 18        CLC
8971: 69 02     ADC #$02
8973: c9 38     CMP #$38
8975: 90 07     BCC $897e
8977: a9 00     LDA #$00
8979: 9d ce 05  STA $05ce,X
897C: f0 06     BEQ $8984
897E: 9d 76 05  STA $0576,X
8981: 20 9a 9a  JSR $9a9a
8984: e8        INX
8985: e0 07     CPX #$07
8987: d0 d7     BNE $8960
8989: a5 90     LDA $90
898B: f0 5a     BEQ $89e7
898D: a9 00     LDA #$00
898F: 85 90     STA $90
8991: a5 ac     LDA $ac
8993: 05 b8     ORA $b8
8995: 05 9a     ORA $9a
8997: d0 4e     BNE $89e7
8999: a5 d9     LDA $d9
899B: d0 05     BNE $89a2
899D: a0 03     LDY #$03
899F: 20 95 b9  JSR $b995
89A2: ca        DEX
89A3: bd ce 05  LDA $05ce,X
89A6: f0 05     BEQ $89ad
89A8: e0 04     CPX #$04
89AA: d0 f6     BNE $89a2
89AC: 60        RTS
89AD: 86 4e     STX $4e
89AF: ae 1e 05  LDX $051e
89B2: a4 92     LDY $92
89B4: b9 94 00  LDA $0094,Y
89B7: 48        PHA
89B8: f0 02     BEQ $89bc
89BA: ca        DEX
89BB: ca        DEX
89BC: 8a        TXA
89BD: 29 7e     AND #$7e
89BF: a6 4e     LDX $4e
89C1: 9d 1e 05  STA $051e,X
89C4: ad 76 05  LDA $0576
89C7: 9d 76 05  STA $0576,X
89CA: 20 9a 9a  JSR $9a9a
89CD: 68        PLA
89CE: f0 17     BEQ $89e7
89D0: ad 1e 05  LDA $051e
89D3: 18        CLC
89D4: 69 02     ADC #$02
89D6: 29 7e     AND #$7e
89D8: 9d 1b 05  STA $051b,X
89DB: bd 76 05  LDA $0576,X
89DE: 9d 73 05  STA $0573,X
89E1: ca        DEX
89E2: ca        DEX
89E3: ca        DEX
89E4: 20 9a 9a  JSR $9a9a
89E7: 60        RTS
89F8: a5 55     LDA $55
89FA: 29 07     AND #$07
89FC: d0 e9     BNE $89e7
89FE: a6 92     LDX $92
8A00: a5 b1     LDA $b1
8A02: f0 e3     BEQ $89e7
8A04: b5 ad     LDA $ad,X
8A06: f0 df     BEQ $89e7
8A08: f6 ad     INC $ad,X
8A0A: c9 07     CMP #$07
8A0C: f0 12     BEQ $8a20
8A0E: b0 2b     BCS $8a3b
8A10: 4a        LSR A
8A11: 90 d4     BCC $89e7
8A13: 4a        LSR A
8A14: b0 d1     BCS $89e7
8A16: c9 03     CMP #$03
8A18: b0 cd     BCS $89e7
8A1A: 85 b3     STA $b3
8A1C: e6 b3     INC $b3
8A1E: d0 c7     BNE $89e7
8A20: a2 03     LDX #$03
8A22: a9 1c     LDA #$1c
8A24: 9d fe 05  STA $05fe,X
8A27: 9d a6 05  STA $05a6,X
8A2A: a9 01     LDA #$01
8A2C: 9d 56 06  STA $0656,X
8A2F: ca        DEX
8A30: d0 f0     BNE $8a22
8A32: 86 b0     STX $b0
8A34: a9 19     LDA #$19
8A36: 85 bb     STA $bb
8A38: 4c 52 8b  JMP $8b52
8A3B: a2 31     LDX #$31
8A3D: bd ce 05  LDA $05ce,X
8A40: d0 03     BNE $8a45
8A42: 4c d2 8a  JMP $8ad2
8A45: 20 f8 9a  JSR $9af8
8A48: bd ce 05  LDA $05ce,X
8A4B: 10 f5     BPL $8a42
8A4D: a9 00     LDA #$00
8A4F: 9d ce 05  STA $05ce,X
8A52: c6 b1     DEC $b1
8A54: e0 32     CPX #$32
8A56: d0 39     BNE $8a91
8A58: a9 01     LDA #$01
8A5A: a4 92     LDY $92
8A5C: b9 94 00  LDA $0094,Y
8A5F: d0 30     BNE $8a91
8A61: a0 04     LDY #$04
8A63: a9 00     LDA #$00
8A65: 99 8e 05  STA $058e,Y
8A68: 88        DEY
8A69: d0 fa     BNE $8a65
8A6B: ad 76 05  LDA $0576
8A6E: 8d 8d 05  STA $058d
8A71: 8d 8e 05  STA $058e
8A74: a9 01     LDA #$01
8A76: 8d 3d 06  STA $063d
8A79: 8d 3e 06  STA $063e
8A7C: ad 1e 05  LDA $051e
8A7F: 69 0c     ADC #$0c
8A81: 29 7e     AND #$7e
8A83: 8d 35 05  STA $0535
8A86: e9 18     SBC #$18
8A88: 29 7e     AND #$7e
8A8A: 8d 36 05  STA $0536
8A8D: a9 c2     LDA #$c2
8A8F: 85 db     STA $db
8A91: a9 0a     LDA #$0a
8A93: 20 2f 98  JSR $982f
8A96: bd 1e 05  LDA $051e,X
8A99: 85 4e     STA $4e
8A9B: bd 76 05  LDA $0576,X
8A9E: 85 4f     STA $4f
8AA0: bd 7e 06  LDA $067e,X
8AA3: a0 00     LDY #$00
8AA5: c9 10     CMP #$10
8AA7: f0 07     BEQ $8ab0
8AA9: a5 b0     LDA $b0
8AAB: 4a        LSR A
8AAC: 4a        LSR A
8AAD: 4a        LSR A
8AAE: a8        TAY
8AAF: c8        INY
8AB0: 20 91 8c  JSR $8c91
8AB3: a0 09     LDY #$09
8AB5: bd 7e 06  LDA $067e,X
8AB8: c9 10     CMP #$10
8ABA: f0 13     BEQ $8acf
8ABC: a5 b0     LDA $b0
8ABE: 20 2f 98  JSR $982f
8AC1: a9 0a     LDA #$0a
8AC3: 18        CLC
8AC4: 65 b0     ADC $b0
8AC6: 85 b0     STA $b0
8AC8: a9 0a     LDA #$0a
8ACA: 20 2f 98  JSR $982f
8ACD: a0 05     LDY #$05
8ACF: 20 95 b9  JSR $b995
8AD2: e8        INX
8AD3: e0 34     CPX #$34
8AD5: f0 03     BEQ $8ada
8AD7: 4c 3d 8a  JMP $8a3d
8ADA: a5 bb     LDA $bb
8ADC: c9 80     CMP #$80
8ADE: 2a        ROL A
8ADF: c9 80     CMP #$80
8AE1: 2a        ROL A
8AE2: 85 bb     STA $bb
8AE4: 29 03     AND #$03
8AE6: 8d 57 06  STA $0657
8AE9: 8d 58 06  STA $0658
8AEC: 8d 59 06  STA $0659
8AEF: a9 00     LDA #$00
8AF1: 85 4e     STA $4e
8AF3: 85 4f     STA $4f
8AF5: a6 92     LDX $92
8AF7: a5 ac     LDA $ac
8AF9: d0 2c     BNE $8b27
8AFB: b5 ad     LDA $ad,X
8AFD: 30 28     BMI $8b27
8AFF: 48        PHA
8B00: 29 02     AND #$02
8B02: d0 02     BNE $8b06
8B04: a9 fe     LDA #$fe
8B06: a8        TAY
8B07: 68        PLA
8B08: 6a        ROR A
8B09: b0 1e     BCS $8b29
8B0B: a5 a3     LDA $a3
8B0D: c9 04     CMP #$04
8B0F: 90 02     BCC $8b13
8B11: a9 03     LDA #$03
8B13: e9 00     SBC #$00
8B15: 29 03     AND #$03
8B17: 69 00     ADC #$00
8B19: 0a        ASL A
8B1A: c0 80     CPY #$80
8B1C: 90 04     BCC $8b22
8B1E: 49 ff     EOR #$ff
8B20: 69 00     ADC #$00
8B22: 85 4f     STA $4f
8B24: 4c 2b 8b  JMP $8b2b
8B27: a0 fc     LDY #$fc
8B29: 84 4e     STY $4e
8B2B: a2 03     LDX #$03
8B2D: bd 4e 05  LDA $054e,X
8B30: 18        CLC
8B31: 65 4f     ADC $4f
8B33: 29 7e     AND #$7e
8B35: 9d 4e 05  STA $054e,X
8B38: bd a6 05  LDA $05a6,X
8B3B: 18        CLC
8B3C: 65 4e     ADC $4e
8B3E: 9d a6 05  STA $05a6,X
8B41: 10 0c     BPL $8b4f
8B43: bc fe 05  LDY $05fe,X
8B46: f0 07     BEQ $8b4f
8B48: a9 00     LDA #$00
8B4A: 9d fe 05  STA $05fe,X
8B4D: c6 b1     DEC $b1
8B4F: ca        DEX
8B50: d0 db     BNE $8b2d
8B52: a2 31     LDX #$31
8B54: bd ce 05  LDA $05ce,X
8B57: f0 0c     BEQ $8b65
8B59: 20 97 9a  JSR $9a97
8B5C: 90 07     BCC $8b65
8B5E: a9 00     LDA #$00
8B60: 9d ce 05  STA $05ce,X
8B63: c6 b1     DEC $b1
8B65: e8        INX
8B66: e0 34     CPX #$34
8B68: d0 ea     BNE $8b54
8B6A: 60        RTS
8B6B: a5 d8     LDA $d8
8B6D: d0 fb     BNE $8b6a
8B6F: a5 55     LDA $55
8B71: 29 07     AND #$07
8B73: d0 17     BNE $8b8c
8B75: a5 b8     LDA $b8
8B77: f0 13     BEQ $8b8c
8B79: 30 0c     BMI $8b87
8B7B: e6 b8     INC $b8
8B7D: e6 b8     INC $b8
8B7F: c9 3c     CMP #$3c
8B81: 90 06     BCC $8b89
8B83: e6 b8     INC $b8
8B85: e6 b8     INC $b8
8B87: a9 3c     LDA #$3c
8B89: 8d 76 05  STA $0576
8B8C: ad 1e 05  LDA $051e
8B8F: 85 b2     STA $b2
8B91: a5 8f     LDA $8f
8B93: c9 0f     CMP #$0f
8B95: d0 04     BNE $8b9b
8B97: a5 b8     LDA $b8
8B99: f0 36     BEQ $8bd1
8B9B: a5 55     LDA $55
8B9D: 4a        LSR A
8B9E: b0 31     BCS $8bd1
8BA0: ac c6 04  LDY $04c6
8BA3: a2 0b     LDX #$0b
8BA5: a9 00     LDA #$00
8BA7: 99 10 22  STA $2210,Y
8BAA: 99 90 22  STA $2290,Y
8BAD: 99 10 23  STA $2310,Y
8BB0: c8        INY
8BB1: ca        DEX
8BB2: d0 f3     BNE $8ba7
8BB4: a6 8f     LDX $8f
8BB6: bd e8 89  LDA $89e8,X
8BB9: 30 13     BMI $8bce
8BBB: 85 b2     STA $b2
8BBD: 85 4e     STA $4e
8BBF: ad 1e 05  LDA $051e
8BC2: 20 bd a0  JSR $a0bd
8BC5: 18        CLC
8BC6: 6d 1e 05  ADC $051e
8BC9: 29 7f     AND #$7f
8BCB: 8d 1e 05  STA $051e
8BCE: 20 14 9c  JSR $9c14
8BD1: a5 55     LDA $55
8BD3: 4a        LSR A
8BD4: b0 3c     BCS $8c12
8BD6: a5 b8     LDA $b8
8BD8: d0 38     BNE $8c12
8BDA: 20 13 8c  JSR $8c13
8BDD: 6d 6e 04  ADC $046e
8BE0: 18        CLC
8BE1: 69 30     ADC #$30
8BE3: 8d 03 d0  STA $d003
8BE6: ac c6 04  LDY $04c6
8BE9: 84 ba     STY $ba
8BEB: ad 1e 05  LDA $051e
8BEE: 69 04     ADC #$04
8BF0: 29 78     AND #$78
8BF2: 85 4e     STA $4e
8BF4: 46 4e     LSR $4e
8BF6: 46 4e     LSR $4e
8BF8: 65 4e     ADC $4e
8BFA: 46 4e     LSR $4e
8BFC: 65 4e     ADC $4e
8BFE: aa        TAX
8BFF: a9 0b     LDA #$0b
8C01: 85 4e     STA $4e
8C03: ad 0a d2  LDA $d20a
8C06: 3d 3b b8  AND $b83b,X
8C09: 99 90 23  STA $2390,Y
8C0C: c8        INY
8C0D: e8        INX
8C0E: c6 4e     DEC $4e
8C10: d0 f1     BNE $8c03
8C12: 60        RTS
8C13: a4 ba     LDY $ba
8C15: a2 0b     LDX #$0b
8C17: a9 00     LDA #$00
8C19: 99 90 23  STA $2390,Y
8C1C: c8        INY
8C1D: ca        DEX
8C1E: d0 f9     BNE $8c19
8C20: 60        RTS
8C21: a2 11     LDX #$11
8C23: bd ce 05  LDA $05ce,X
8C26: f0 40     BEQ $8c68
8C28: 30 0e     BMI $8c38
8C2A: d6 32     DEC $32,X
8C2C: d0 3a     BNE $8c68
8C2E: 20 f8 9a  JSR $9af8
8C31: a9 00     LDA #$00
8C33: 9d ce 05  STA $05ce,X
8C36: f0 2a     BEQ $8c62
8C38: de ce 05  DEC $05ce,X
8C3B: 30 2b     BMI $8c68
8C3D: a9 78     LDA #$78
8C3F: 95 32     STA $32,X
8C41: 20 97 9a  JSR $9a97
8C44: a9 00     LDA #$00
8C46: 9d 29 06  STA $0629,X
8C49: a9 06     LDA #$06
8C4B: 9d d1 05  STA $05d1,X
8C4E: a9 28     LDA #$28
8C50: 9d 81 06  STA $0681,X
8C53: bd c6 04  LDA $04c6,X
8C56: 9d c9 04  STA $04c9,X
8C59: bd 6e 04  LDA $046e,X
8C5C: 18        CLC
8C5D: 69 08     ADC #$08
8C5F: 9d 71 04  STA $0471,X
8C62: e8        INX
8C63: e8        INX
8C64: e8        INX
8C65: 4c f8 9a  JMP $9af8
8C68: e8        INX
8C69: e0 14     CPX #$14
8C6B: d0 b6     BNE $8c23
8C6D: 60        RTS
8C6E: a2 08     LDX #$08
8C70: a9 00     LDA #$00
8C72: 9d ff d1  STA $d1ff,X
8C75: ca        DEX
8C76: d0 fa     BNE $8c72
8C78: 60        RTS
8C79: 20 6a 88  JSR $886a
8C7C: 20 05 84  JSR $8405
8C7F: a9 00     LDA #$00
8C81: 8d 00 d4  STA $d400
8C84: 60        RTS
8C85: 20 05 84  JSR $8405
8C88: a9 2e     LDA #$2e
8C8A: 8d 00 d4  STA $d400
8C8D: 20 6e 88  JSR $886e
8C90: 60        RTS
8C91: 8a        TXA
8C92: 48        PHA
8C93: a2 11     LDX #$11
8C95: bd ce 05  LDA $05ce,X
8C98: f0 07     BEQ $8ca1
8C9A: e8        INX
8C9B: e0 14     CPX #$14
8C9D: d0 f6     BNE $8c95
8C9F: f0 20     BEQ $8cc1
8CA1: a5 4e     LDA $4e
8CA3: 69 04     ADC #$04
8CA5: 29 7e     AND #$7e
8CA7: 9d 1e 05  STA $051e,X
8CAA: a5 4f     LDA $4f
8CAC: c9 12     CMP #$12
8CAE: b0 02     BCS $8cb2
8CB0: a9 12     LDA #$12
8CB2: 9d 76 05  STA $0576,X
8CB5: 98        TYA
8CB6: 0a        ASL A
8CB7: 69 2a     ADC #$2a
8CB9: 9d 7e 06  STA $067e,X
8CBC: a9 9e     LDA #$9e
8CBE: 9d ce 05  STA $05ce,X
8CC1: 68        PLA
8CC2: aa        TAX
8CC3: 60        RTS
8CC4: a9 03     LDA #$03
8CC6: a6 ac     LDX $ac
8CC8: f0 02     BEQ $8ccc
8CCA: a9 07     LDA #$07
8CCC: 25 55     AND $55
8CCE: d0 3b     BNE $8d0b
8CD0: a2 1d     LDX #$1d
8CD2: c6 d8     DEC $d8
8CD4: 20 5b 88  JSR $885b
8CD7: 24 d8     BIT $d8
8CD9: 30 03     BMI $8cde
8CDB: 20 0c 8d  JSR $8d0c
8CDE: a5 d8     LDA $d8
8CE0: f0 15     BEQ $8cf7
8CE2: bd 6e 04  LDA $046e,X
8CE5: 18        CLC
8CE6: 75 b1     ADC $b1,X
8CE8: 9d 6e 04  STA $046e,X
8CEB: bd c6 04  LDA $04c6,X
8CEE: 18        CLC
8CEF: 75 b6     ADC $b6,X
8CF1: 9d c6 04  STA $04c6,X
8CF4: 20 0c 8d  JSR $8d0c
8CF7: e8        INX
8CF8: e0 22     CPX #$22
8CFA: d0 d8     BNE $8cd4
8CFC: a5 d8     LDA $d8
8CFE: 29 7f     AND #$7f
8D00: 85 d8     STA $d8
8D02: d0 07     BNE $8d0b
8D04: a5 ac     LDA $ac
8D06: d0 03     BNE $8d0b
8D08: 20 14 9c  JSR $9c14
8D0B: 60        RTS
8D0C: bd 6e 04  LDA $046e,X
8D0F: c9 9d     CMP #$9d
8D11: b0 0a     BCS $8d1d
8D13: bd c6 04  LDA $04c6,X
8D16: c9 5d     CMP #$5d
8D18: b0 03     BCS $8d1d
8D1A: 20 fb 9a  JSR $9afb
8D1D: 60        RTS
8D1E: a4 b8     LDY $b8
8D20: d0 05     BNE $8d27
8D22: a5 55     LDA $55
8D24: 4a        LSR A
8D25: b0 f6     BCS $8d1d
8D27: a2 14     LDX #$14
8D29: bd e7 20  LDA $20e7,X
8D2C: 10 17     BPL $8d45
8D2E: ad 0a d2  LDA $d20a
8D31: 29 07     AND #$07
8D33: d0 78     BNE $8dad
8D35: a9 24     LDA #$24
8D37: 9d fb 20  STA $20fb,X
8D3A: ad 0a d2  LDA $d20a
8D3D: 29 7e     AND #$7e
8D3F: 9d e7 20  STA $20e7,X
8D42: 4c 78 8d  JMP $8d78
8D45: bd fb 20  LDA $20fb,X
8D48: 2a        ROL A
8D49: 2a        ROL A
8D4A: 2a        ROL A
8D4B: 29 03     AND #$03
8D4D: a8        TAY
8D4E: bd bf 20  LDA $20bf,X
8D51: 85 09     STA $09
8D53: bd d3 20  LDA $20d3,X
8D56: 85 0a     STA $0a
8D58: b9 be 8d  LDA $8dbe,Y
8D5B: a0 00     LDY #$00
8D5D: 51 09     EOR ($09),Y
8D5F: 91 09     STA ($09),Y
8D61: bd fb 20  LDA $20fb,X
8D64: 29 3f     AND #$3f
8D66: 38        SEC
8D67: e9 02     SBC #$02
8D69: 9d fb 20  STA $20fb,X
8D6C: 90 44     BCC $8db2
8D6E: a4 f9     LDY $f9
8D70: c0 02     CPY #$02
8D72: d0 04     BNE $8d78
8D74: c9 12     CMP #$12
8D76: 90 3a     BCC $8db2
8D78: 8a        TXA
8D79: 48        PHA
8D7A: bd e7 20  LDA $20e7,X
8D7D: 85 8b     STA $8b
8D7F: bd fb 20  LDA $20fb,X
8D82: 85 8c     STA $8c
8D84: 20 cb a0  JSR $a0cb
8D87: b0 27     BCS $8db0
8D89: 20 8a 87  JSR $878a
8D8C: a6 4e     LDX $4e
8D8E: bd be 8d  LDA $8dbe,X
8D91: 51 09     EOR ($09),Y
8D93: 91 09     STA ($09),Y
8D95: 68        PLA
8D96: aa        TAX
8D97: 18        CLC
8D98: a5 4e     LDA $4e
8D9A: 6a        ROR A
8D9B: 6a        ROR A
8D9C: 6a        ROR A
8D9D: 1d fb 20  ORA $20fb,X
8DA0: 9d fb 20  STA $20fb,X
8DA3: a5 09     LDA $09
8DA5: 9d bf 20  STA $20bf,X
8DA8: a5 0a     LDA $0a
8DAA: 9d d3 20  STA $20d3,X
8DAD: 4c b7 8d  JMP $8db7
8DB0: 68        PLA
8DB1: aa        TAX
8DB2: a9 ff     LDA #$ff
8DB4: 9d e7 20  STA $20e7,X
8DB7: ca        DEX
8DB8: f0 03     BEQ $8dbd
8DBA: 4c 29 8d  JMP $8d29
8DBD: 60        RTS
8DC2: a0 00     LDY #$00
8DC4: bd d1 8d  LDA $8dd1,X
8DC7: 99 16 d0  STA $d016,Y
8DCA: e8        INX
8DCB: c8        INY
8DCC: c0 03     CPY #$03
8DCE: d0 f4     BNE $8dc4
8DD0: 60        RTS
8DE3: a9 8d     LDA #$8d
8DE5: e0 12     CPX #$12
8DE7: f0 02     BEQ $8deb
8DE9: a9 0d     LDA #$0d
8DEB: 8d 31 04  STA $0431
8DEE: a0 00     LDY #$00
8DF0: 20 05 84  JSR $8405
8DF3: bd 06 8e  LDA $8e06,X
8DF6: 99 45 04  STA $0445,Y
8DF9: bd 2a 8e  LDA $8e2a,X
8DFC: 99 56 04  STA $0456,Y
8DFF: e8        INX
8E00: c8        INY
8E01: c0 09     CPY #$09
8E03: d0 ee     BNE $8df3
8E05: 60        RTS
8E62: 20 8a 84  JSR $848a
8E65: 20 a6 84  JSR $84a6
8E68: 20 85 84  JSR $8485
8E6B: a2 06     LDX #$06
8E6D: a9 22     LDA #$22
8E6F: 9d 94 06  STA $0694,X
8E72: a9 00     LDA #$00
8E74: 9d 7e 06  STA $067e,X
8E77: ca        DEX
8E78: d0 f3     BNE $8e6d
8E7A: a9 44     LDA #$44
8E7C: 8d 14 d0  STA $d014
8E7F: a9 0e     LDA #$0e
8E81: 8d 12 d0  STA $d012
8E84: 8d 13 d0  STA $d013
8E87: a2 01     LDX #$01
8E89: 86 92     STX $92
8E8B: a9 00     LDA #$00
8E8D: 85 d8     STA $d8
8E8F: 85 bc     STA $bc
8E91: 85 bd     STA $bd
8E93: 85 98     STA $98
8E95: 85 ac     STA $ac
8E97: 85 5b     STA $5b
8E99: 85 90     STA $90
8E9B: 85 93     STA $93
8E9D: 85 9a     STA $9a
8E9F: 95 94     STA $94,X
8EA1: 95 9d     STA $9d,X
8EA3: 95 ad     STA $ad,X
8EA5: ca        DEX
8EA6: 10 f7     BPL $8e9f
8EA8: a2 0c     LDX #$0c
8EAA: 9d 0f 21  STA $210f,X
8EAD: ca        DEX
8EAE: 10 fa     BPL $8eaa
8EB0: 20 2f 98  JSR $982f
8EB3: c6 92     DEC $92
8EB5: a9 00     LDA #$00
8EB7: 20 2f 98  JSR $982f
8EBA: a0 0c     LDY #$0c
8EBC: a9 34     LDA #$34
8EBE: 85 4e     STA $4e
8EC0: a2 00     LDX #$00
8EC2: a9 16     LDA #$16
8EC4: 9d b2 06  STA $06b2,X
8EC7: b9 e3 8e  LDA $8ee3,Y
8ECA: 9d 52 05  STA $0552,X
8ECD: a5 4e     LDA $4e
8ECF: 9d aa 05  STA $05aa,X
8ED2: 88        DEY
8ED3: d0 09     BNE $8ede
8ED5: a0 0c     LDY #$0c
8ED7: a5 4e     LDA $4e
8ED9: 38        SEC
8EDA: e9 04     SBC #$04
8EDC: 85 4e     STA $4e
8EDE: e8        INX
8EDF: e0 24     CPX #$24
8EE1: d0 df     BNE $8ec2
8EE3: 60        RTS
8EF0: 20 79 8c  JSR $8c79
8EF3: 20 27 88  JSR $8827
8EF6: a2 12     LDX #$12
8EF8: 86 f9     STX $f9
8EFA: 20 e3 8d  JSR $8de3
8EFD: 20 85 84  JSR $8485
8F00: 20 8a 84  JSR $848a
8F03: 20 a6 84  JSR $84a6
8F06: a9 00     LDA #$00
8F08: 20 01 b9  JSR $b901
8F0B: a9 09     LDA #$09
8F0D: 85 46     STA $46
8F0F: a9 a9     LDA #$a9
8F11: 85 47     STA $47
8F13: a9 0b     LDA #$0b
8F15: 85 09     STA $09
8F17: a9 32     LDA #$32
8F19: 85 0a     STA $0a
8F1B: a2 22     LDX #$22
8F1D: a0 19     LDY #$19
8F1F: b1 46     LDA ($46),Y
8F21: 91 09     STA ($09),Y
8F23: 88        DEY
8F24: 10 f9     BPL $8f1f
8F26: a5 09     LDA $09
8F28: 18        CLC
8F29: 69 28     ADC #$28
8F2B: 85 09     STA $09
8F2D: 90 03     BCC $8f32
8F2F: e6 0a     INC $0a
8F31: 18        CLC
8F32: a5 46     LDA $46
8F34: 69 1a     ADC #$1a
8F36: 85 46     STA $46
8F38: 90 02     BCC $8f3c
8F3A: e6 47     INC $47
8F3C: ca        DEX
8F3D: d0 de     BNE $8f1d
8F3F: 8a        TXA
8F40: 85 57     STA $57
8F42: 85 92     STA $92
8F44: e6 92     INC $92
8F46: 20 2f 98  JSR $982f
8F49: 8a        TXA
8F4A: c6 92     DEC $92
8F4C: 20 2f 98  JSR $982f
8F4F: 20 38 9f  JSR $9f38
8F52: 4c 85 8c  JMP $8c85
8F65: bd 55 8f  LDA $8f55,X
8F68: 85 09     STA $09
8F6A: bd 56 8f  LDA $8f56,X
8F6D: 85 0a     STA $0a
8F6F: a9 00     LDA #$00
8F71: a8        TAY
8F72: 91 09     STA ($09),Y
8F74: a5 09     LDA $09
8F76: dd 57 8f  CMP $8f57,X
8F79: d0 07     BNE $8f82
8F7B: a5 0a     LDA $0a
8F7D: dd 58 8f  CMP $8f58,X
8F80: f0 0b     BEQ $8f8d
8F82: a5 09     LDA $09
8F84: d0 02     BNE $8f88
8F86: c6 0a     DEC $0a
8F88: c6 09     DEC $09
8F8A: 4c 6f 8f  JMP $8f6f
8F8D: 60        RTS
8F8E: a2 09     LDX #$09
8F90: a5 f2     LDA $f2
8F92: 85 f1     STA $f1
8F94: d0 06     BNE $8f9c
8F96: a2 08     LDX #$08
8F98: a5 f0     LDA $f0
8F9A: 85 ef     STA $ef
8F9C: b5 24     LDA $24,X
8F9E: f0 15     BEQ $8fb5
8FA0: b5 1a     LDA $1a,X
8FA2: f0 14     BEQ $8fb8
8FA4: 10 15     BPL $8fbb
8FA6: a5 ac     LDA $ac
8FA8: f0 09     BEQ $8fb3
8FAA: e6 f5     INC $f5
8FAC: a5 f5     LDA $f5
8FAE: 95 2e     STA $2e,X
8FB0: 4c 75 90  JMP $9075
8FB3: f6 1a     INC $1a,X
8FB5: 4c 27 92  JMP $9227
8FB8: 4c 8a 90  JMP $908a
8FBB: 20 ca 92  JSR $92ca
8FBE: bd f5 05  LDA $05f5,X
8FC1: 10 63     BPL $9026
8FC3: a9 02     LDA #$02
8FC5: 20 2f 98  JSR $982f
8FC8: a5 da     LDA $da
8FCA: f0 02     BEQ $8fce
8FCC: c6 da     DEC $da
8FCE: a5 ea     LDA $ea
8FD0: f0 09     BEQ $8fdb
8FD2: 86 58     STX $58
8FD4: a0 05     LDY #$05
8FD6: 20 95 b9  JSR $b995
8FD9: a6 58     LDX $58
8FDB: a4 99     LDY $99
8FDD: 88        DEY
8FDE: d0 43     BNE $9023
8FE0: a5 e6     LDA $e6
8FE2: f0 08     BEQ $8fec
8FE4: a5 96     LDA $96
8FE6: d0 3b     BNE $9023
8FE8: a9 82     LDA #$82
8FEA: 85 cd     STA $cd
8FEC: a5 cd     LDA $cd
8FEE: 10 33     BPL $9023
8FF0: 29 7f     AND #$7f
8FF2: 85 4e     STA $4e
8FF4: a9 0a     LDA #$0a
8FF6: 20 2f 98  JSR $982f
8FF9: c6 4e     DEC $4e
8FFB: d0 f7     BNE $8ff4
8FFD: a5 cd     LDA $cd
8FFF: 29 7f     AND #$7f
9001: a8        TAY
9002: 88        DEY
9003: bd 45 05  LDA $0545,X
9006: 85 4e     STA $4e
9008: bd 9d 05  LDA $059d,X
900B: 85 4f     STA $4f
900D: 20 91 8c  JSR $8c91
9010: a0 07     LDY #$07
9012: 20 95 b9  JSR $b995
9015: a5 d9     LDA $d9
9017: f0 0a     BEQ $9023
9019: a5 cd     LDA $cd
901B: 29 7f     AND #$7f
901D: c9 08     CMP #$08
901F: b0 02     BCS $9023
9021: e6 cd     INC $cd
9023: 4c a7 90  JMP $90a7
9026: b5 24     LDA $24,X
9028: 30 60     BMI $908a
902A: 0a        ASL A
902B: 10 03     BPL $9030
902D: 4c 0e 91  JMP $910e
9030: c9 54     CMP #$54
9032: b0 0c     BCS $9040
9034: f6 24     INC $24,X
9036: f6 24     INC $24,X
9038: b5 24     LDA $24,X
903A: 9d 9d 05  STA $059d,X
903D: 4c 74 91  JMP $9174
9040: b4 2e     LDY $2e,X
9042: bd 45 05  LDA $0545,X
9045: 38        SEC
9046: f9 51 05  SBC $0551,Y
9049: f0 2a     BEQ $9075
904B: 29 40     AND #$40
904D: d0 04     BNE $9053
904F: a9 0c     LDA #$0c
9051: d0 02     BNE $9055
9053: a9 08     LDA #$08
9055: 9d a5 06  STA $06a5,X
9058: a9 0a     LDA #$0a
905A: 38        SEC
905B: fd a5 06  SBC $06a5,X
905E: 18        CLC
905F: 7d 45 05  ADC $0545,X
9062: 29 7f     AND #$7f
9064: 9d 45 05  STA $0545,X
9067: d9 51 05  CMP $0551,Y
906A: f0 09     BEQ $9075
906C: 4a        LSR A
906D: 4a        LSR A
906E: 4a        LSR A
906F: 9d 4d 06  STA $064d,X
9072: 4c 74 91  JMP $9174
9075: 06 cd     ASL $cd
9077: 46 cd     LSR $cd
9079: c6 99     DEC $99
907B: e6 96     INC $96
907D: a9 00     LDA #$00
907F: 9d f5 05  STA $05f5,X
9082: 95 24     STA $24,X
9084: 20 2f 92  JSR $922f
9087: 4c 27 92  JMP $9227
908A: 8a        TXA
908B: 6a        ROR A
908C: 6a        ROR A
908D: 29 80     AND #$80
908F: 15 1a     ORA $1a,X
9091: f6 1a     INC $1a,X
9093: a8        TAY
9094: b9 d6 08  LDA $08d6,Y
9097: c9 ff     CMP #$ff
9099: d0 2f     BNE $90ca
909B: a9 03     LDA #$03
909D: 85 f9     STA $f9
909F: 06 cd     ASL $cd
90A1: 46 cd     LSR $cd
90A3: a5 d9     LDA $d9
90A5: d0 0b     BNE $90b2
90A7: c6 99     DEC $99
90A9: a9 00     LDA #$00
90AB: 9d f5 05  STA $05f5,X
90AE: 95 24     STA $24,X
90B0: f0 d5     BEQ $9087
90B2: bd 9d 05  LDA $059d,X
90B5: 18        CLC
90B6: 69 02     ADC #$02
90B8: 9d 9d 05  STA $059d,X
90BB: 95 24     STA $24,X
90BD: e6 f5     INC $f5
90BF: a5 f5     LDA $f5
90C1: 95 2e     STA $2e,X
90C3: a9 06     LDA #$06
90C5: 9d a5 06  STA $06a5,X
90C8: d0 6d     BNE $9137
90CA: 85 4e     STA $4e
90CC: a5 ac     LDA $ac
90CE: f0 24     BEQ $90f4
90D0: bd 45 05  LDA $0545,X
90D3: bc a5 06  LDY $06a5,X
90D6: c0 08     CPY #$08
90D8: d0 04     BNE $90de
90DA: 69 01     ADC #$01
90DC: d0 06     BNE $90e4
90DE: c0 0c     CPY #$0c
90E0: d0 02     BNE $90e4
90E2: e9 02     SBC #$02
90E4: 29 7f     AND #$7f
90E6: 9d 45 05  STA $0545,X
90E9: de 9d 05  DEC $059d,X
90EC: de 9d 05  DEC $059d,X
90EF: 10 68     BPL $9159
90F1: 4c a7 90  JMP $90a7
90F4: a5 4e     LDA $4e
90F6: 29 0f     AND #$0f
90F8: d0 62     BNE $915c
90FA: a5 4e     LDA $4e
90FC: 4a        LSR A
90FD: 4a        LSR A
90FE: 4a        LSR A
90FF: 4a        LSR A
9100: 85 4e     STA $4e
9102: e4 4e     CPX $4e
9104: d0 84     BNE $908a
9106: a9 03     LDA #$03
9108: 85 f9     STA $f9
910A: a9 40     LDA #$40
910C: 95 24     STA $24,X
910E: a5 a3     LDA $a3
9110: c9 07     CMP #$07
9112: b0 09     BCS $911d
9114: b5 24     LDA $24,X
9116: 49 01     EOR #$01
9118: 95 24     STA $24,X
911A: 4a        LSR A
911B: 90 3c     BCC $9159
911D: de 9d 05  DEC $059d,X
9120: de 9d 05  DEC $059d,X
9123: 30 71     BMI $9196
9125: ad 1e 05  LDA $051e
9128: 85 4e     STA $4e
912A: bd 45 05  LDA $0545,X
912D: 20 bd a0  JSR $a0bd
9130: d0 07     BNE $9139
9132: a9 0a     LDA #$0a
9134: 9d a5 06  STA $06a5,X
9137: d0 20     BNE $9159
9139: 85 4e     STA $4e
913B: a9 0a     LDA #$0a
913D: 38        SEC
913E: e5 4e     SBC $4e
9140: 9d a5 06  STA $06a5,X
9143: bd 45 05  LDA $0545,X
9146: 18        CLC
9147: 65 4e     ADC $4e
9149: 29 7f     AND #$7f
914B: 9d 45 05  STA $0545,X
914E: 4a        LSR A
914F: 4a        LSR A
9150: 4a        LSR A
9151: 38        SEC
9152: e5 4e     SBC $4e
9154: 29 0f     AND #$0f
9156: 9d 4d 06  STA $064d,X
9159: 4c 74 91  JMP $9174
915C: 9d a5 06  STA $06a5,X
915F: a5 4e     LDA $4e
9161: 4a        LSR A
9162: 4a        LSR A
9163: 4a        LSR A
9164: 4a        LSR A
9165: 9d 4d 06  STA $064d,X
9168: b9 d6 06  LDA $06d6,Y
916B: 9d 9d 05  STA $059d,X
916E: b9 d6 07  LDA $07d6,Y
9171: 9d 45 05  STA $0545,X
9174: 8a        TXA
9175: 48        PHA
9176: 18        CLC
9177: 69 27     ADC #$27
9179: aa        TAX
917A: 20 97 9a  JSR $9a97
917D: 68        PLA
917E: aa        TAX
917F: 90 18     BCC $9199
9181: a5 d9     LDA $d9
9183: f0 11     BEQ $9196
9185: a5 e6     LDA $e6
9187: f0 0a     BEQ $9193
9189: ad 0a d2  LDA $d20a
918C: 29 03     AND #$03
918E: f0 06     BEQ $9196
9190: 4c 75 90  JMP $9075
9193: 4c aa 8f  JMP $8faa
9196: 4c a7 90  JMP $90a7
9199: b5 dc     LDA $dc,X
919B: f0 30     BEQ $91cd
919D: 20 40 92  JSR $9240
91A0: b5 dc     LDA $dc,X
91A2: c9 02     CMP #$02
91A4: f0 41     BEQ $91e7
91A6: 90 28     BCC $91d0
91A8: ad 0a d2  LDA $d20a
91AB: 25 e7     AND $e7
91AD: f0 75     BEQ $9224
91AF: b9 d6 08  LDA $08d6,Y
91B2: 29 0f     AND #$0f
91B4: 85 4e     STA $4e
91B6: 38        SEC
91B7: e9 0a     SBC #$0a
91B9: 20 08 98  JSR $9808
91BC: 18        CLC
91BD: 7d 45 05  ADC $0545,X
91C0: 29 7f     AND #$7f
91C2: 99 d6 07  STA $07d6,Y
91C5: 0a        ASL A
91C6: 29 f0     AND #$f0
91C8: 05 4e     ORA $4e
91CA: 99 d6 08  STA $08d6,Y
91CD: 4c 27 92  JMP $9227
91D0: bd 9d 05  LDA $059d,X
91D3: c9 10     CMP #$10
91D5: d0 50     BNE $9227
91D7: f6 dc     INC $dc,X
91D9: bd a5 06  LDA $06a5,X
91DC: c9 0a     CMP #$0a
91DE: d0 07     BNE $91e7
91E0: a9 00     LDA #$00
91E2: 95 dc     STA $dc,X
91E4: 4c 27 92  JMP $9227
91E7: a9 ff     LDA #$ff
91E9: 95 24     STA $24,X
91EB: bd 9d 05  LDA $059d,X
91EE: c9 1e     CMP #$1e
91F0: f0 32     BEQ $9224
91F2: bd a5 06  LDA $06a5,X
91F5: 38        SEC
91F6: e9 0a     SBC #$0a
91F8: 20 08 98  JSR $9808
91FB: 85 4e     STA $4e
91FD: bd 9d 05  LDA $059d,X
9200: 18        CLC
9201: 69 02     ADC #$02
9203: 99 d6 06  STA $06d6,Y
9206: bd 45 05  LDA $0545,X
9209: 18        CLC
920A: 65 4e     ADC $4e
920C: 29 7f     AND #$7f
920E: 99 d6 07  STA $07d6,Y
9211: 4a        LSR A
9212: 4a        LSR A
9213: 4a        LSR A
9214: 38        SEC
9215: e5 4e     SBC $4e
9217: 0a        ASL A
9218: 0a        ASL A
9219: 0a        ASL A
921A: 0a        ASL A
921B: 1d a5 06  ORA $06a5,X
921E: 99 d6 08  STA $08d6,Y
9221: 4c 27 92  JMP $9227
9224: 20 4f 92  JSR $924f
9227: ca        DEX
9228: ca        DEX
9229: 30 03     BMI $922e
922B: 4c 9c 8f  JMP $8f9c
922E: 60        RTS
922F: 8a        TXA
9230: 48        PHA
9231: b5 2e     LDA $2e,X
9233: 18        CLC
9234: 69 33     ADC #$33
9236: aa        TAX
9237: 20 af 87  JSR $87af
923A: 20 9a 9a  JSR $9a9a
923D: 68        PLA
923E: aa        TAX
923F: 60        RTS
9240: 8a        TXA
9241: 4a        LSR A
9242: 90 03     BCC $9247
9244: 09 80     ORA #$80
9246: 18        CLC
9247: 69 01     ADC #$01
9249: a8        TAY
924A: 29 7f     AND #$7f
924C: 95 1a     STA $1a,X
924E: 60        RTS
924F: a9 ff     LDA #$ff
9251: 95 24     STA $24,X
9253: ad 0a d2  LDA $d20a
9256: 29 0f     AND #$0f
9258: f0 04     BEQ $925e
925A: a9 00     LDA #$00
925C: f0 05     BEQ $9263
925E: ad 0a d2  LDA $d20a
9261: 29 0e     AND #$0e
9263: a8        TAY
9264: b9 d6 92  LDA $92d6,Y
9267: 85 09     STA $09
9269: b9 d7 92  LDA $92d7,Y
926C: 85 0a     STA $0a
926E: 20 40 92  JSR $9240
9271: 6c 09 00  JMP ($0009)
9274: de 9d 05  DEC $059d,X
9277: de 9d 05  DEC $059d,X
927A: bd 9d 05  LDA $059d,X
927D: c9 11     CMP #$11
927F: 90 32     BCC $92b3
9281: ad 1e 05  LDA $051e
9284: 85 4e     STA $4e
9286: bd 45 05  LDA $0545,X
9289: 20 bd a0  JSR $a0bd
928C: d0 02     BNE $9290
928E: a9 02     LDA #$02
9290: 38        SEC
9291: e9 0a     SBC #$0a
9293: 20 08 98  JSR $9808
9296: 85 4e     STA $4e
9298: 4a        LSR A
9299: 4a        LSR A
929A: 69 01     ADC #$01
929C: 95 dc     STA $dc,X
929E: bd 45 05  LDA $0545,X
92A1: 99 d6 07  STA $07d6,Y
92A4: 0a        ASL A
92A5: 29 f0     AND #$f0
92A7: 05 4e     ORA $4e
92A9: 99 d6 08  STA $08d6,Y
92AC: bd 9d 05  LDA $059d,X
92AF: 99 d6 06  STA $06d6,Y
92B2: 60        RTS
92B3: a9 00     LDA #$00
92B5: f0 02     BEQ $92b9
92B7: a9 01     LDA #$01
92B9: 95 dc     STA $dc,X
92BB: a9 40     LDA #$40
92BD: 95 24     STA $24,X
92BF: 60        RTS
92CA: 8a        TXA
92CB: 48        PHA
92CC: 18        CLC
92CD: 69 27     ADC #$27
92CF: aa        TAX
92D0: 20 f8 9a  JSR $9af8
92D3: 68        PLA
92D4: aa        TAX
92D5: 60        RTS
92E6: a2 0a     LDX #$0a
92E8: a9 00     LDA #$00
92EA: 85 b4     STA $b4
92EC: 95 db     STA $db,X
92EE: 95 23     STA $23,X
92F0: ca        DEX
92F1: d0 f9     BNE $92ec
92F3: a6 92     LDX $92
92F5: b5 9d     LDA $9d,X
92F7: 4a        LSR A
92F8: 4a        LSR A
92F9: 4a        LSR A
92FA: 69 02     ADC #$02
92FC: 85 cd     STA $cd
92FE: a9 28     LDA #$28
9300: 85 da     STA $da
9302: a9 0e     LDA #$0e
9304: 8d 13 d0  STA $d013
9307: 20 a6 84  JSR $84a6
930A: a2 05     LDX #$05
930C: bd 13 94  LDA $9413,X
930F: 9d e3 3f  STA $3fe3,X
9312: ca        DEX
9313: d0 f7     BNE $930c
9315: a9 44     LDA #$44
9317: 8d 19 d0  STA $d019
931A: a6 92     LDX $92
931C: b5 a3     LDA $a3,X
931E: 4a        LSR A
931F: 4a        LSR A
9320: 48        PHA
9321: c9 04     CMP #$04
9323: 90 02     BCC $9327
9325: a9 03     LDA #$03
9327: aa        TAX
9328: bd 19 94  LDA $9419,X
932B: 85 e7     STA $e7
932D: bd 1d 94  LDA $941d,X
9330: 85 e9     STA $e9
9332: bd 21 94  LDA $9421,X
9335: 85 f6     STA $f6
9337: 68        PLA
9338: 18        CLC
9339: 69 02     ADC #$02
933B: c9 05     CMP #$05
933D: 90 02     BCC $9341
933F: a9 04     LDA #$04
9341: 85 e8     STA $e8
9343: a5 a3     LDA $a3
9345: 29 03     AND #$03
9347: 85 4e     STA $4e
9349: 0a        ASL A
934A: 65 4e     ADC $4e
934C: 69 06     ADC #$06
934E: aa        TAX
934F: 20 c2 8d  JSR $8dc2
9352: 20 f0 99  JSR $99f0
9355: 20 bc 98  JSR $98bc
9358: a9 00     LDA #$00
935A: 85 e6     STA $e6
935C: 85 db     STA $db
935E: 20 2f 98  JSR $982f
9361: 20 85 84  JSR $8485
9364: a9 84     LDA #$84
9366: 8d 15 d0  STA $d015
9369: 20 8a 84  JSR $848a
936C: 8d 1e d0  STA $d01e
936F: a4 92     LDY $92
9371: b9 9b 00  LDA $009b,Y
9374: 85 d9     STA $d9
9376: 4a        LSR A
9377: 90 02     BCC $937b
9379: 69 01     ADC #$01
937B: 69 02     ADC #$02
937D: 20 01 b9  JSR $b901
9380: a9 03     LDA #$03
9382: a4 92     LDY $92
9384: a6 d9     LDX $d9
9386: f0 06     BEQ $938e
9388: b6 ad     LDX $ad,Y
938A: f0 07     BEQ $9393
938C: d0 03     BNE $9391
938E: 99 ad 00  STA $00ad,Y
9391: a9 00     LDA #$00
9393: 85 b1     STA $b1
9395: b9 a3 00  LDA $00a3,Y
9398: 18        CLC
9399: 69 01     ADC #$01
939B: 4a        LSR A
939C: c9 05     CMP #$05
939E: 90 02     BCC $93a2
93A0: a9 04     LDA #$04
93A2: 85 a6     STA $a6
93A4: 0a        ASL A
93A5: 0a        ASL A
93A6: 0a        ASL A
93A7: e9 41     SBC #$41
93A9: 20 08 98  JSR $9808
93AC: 85 a8     STA $a8
93AE: a2 00     LDX #$00
93B0: 86 b8     STX $b8
93B2: 86 b3     STX $b3
93B4: 86 96     STX $96
93B6: 86 90     STX $90
93B8: 86 b7     STX $b7
93BA: 86 99     STX $99
93BC: 86 9f     STX $9f
93BE: 8e 1e 05  STX $051e
93C1: 86 f5     STX $f5
93C3: 86 a9     STX $a9
93C5: a6 92     LDX $92
93C7: b5 a1     LDA $a1,X
93C9: 85 a0     STA $a0
93CB: a5 ac     LDA $ac
93CD: 10 1f     BPL $93ee
93CF: b5 97     LDA $97,X
93D1: 85 96     STA $96
93D3: f0 15     BEQ $93ea
93D5: 48        PHA
93D6: e6 f5     INC $f5
93D8: a5 f5     LDA $f5
93DA: 18        CLC
93DB: 69 33     ADC #$33
93DD: aa        TAX
93DE: 20 af 87  JSR $87af
93E1: 20 9a 9a  JSR $9a9a
93E4: 68        PLA
93E5: 38        SEC
93E6: e9 01     SBC #$01
93E8: d0 eb     BNE $93d5
93EA: a2 14     LDX #$14
93EC: d0 08     BNE $93f6
93EE: a2 00     LDX #$00
93F0: a5 d9     LDA $d9
93F2: d0 02     BNE $93f6
93F4: a2 28     LDX #$28
93F6: 20 de 98  JSR $98de
93F9: a9 0c     LDA #$0c
93FB: 8d 76 05  STA $0576
93FE: 85 8c     STA $8c
9400: a9 00     LDA #$00
9402: 85 ac     STA $ac
9404: 85 8b     STA $8b
9406: 20 cb a0  JSR $a0cb
9409: 8c c6 04  STY $04c6
940C: 8d 6e 04  STA $046e
940F: a0 00     LDY #$00
9411: 4c 1d a0  JMP $a01d
9425: 20 48 94  JSR $9448
9428: a9 00     LDA #$00
942A: 95 a1     STA $a1,X
942C: 95 ad     STA $ad,X
942E: f6 a3     INC $a3,X
9430: d6 9b     DEC $9b,X
9432: 10 11     BPL $9445
9434: a9 03     LDA #$03
9436: 95 9b     STA $9b,X
9438: b5 9d     LDA $9d,X
943A: 18        CLC
943B: 69 08     ADC #$08
943D: c9 30     CMP #$30
943F: d0 02     BNE $9443
9441: a9 00     LDA #$00
9443: 95 9d     STA $9d,X
9445: 4c e6 92  JMP $92e6
9448: a6 92     LDX $92
944A: b5 a3     LDA $a3,X
944C: 4a        LSR A
944D: 4a        LSR A
944E: 38        SEC
944F: e9 05     SBC #$05
9451: b0 07     BCS $945a
9453: 20 08 98  JSR $9808
9456: c9 01     CMP #$01
9458: b0 02     BCS $945c
945A: a9 02     LDA #$02
945C: 95 f7     STA $f7,X
945E: 60        RTS
9634: 06 cd     ASL $cd
9636: 38        SEC
9637: 66 cd     ROR $cd
9639: a2 08     LDX #$08
963B: 86 99     STX $99
963D: a9 ff     LDA #$ff
963F: 95 23     STA $23,X
9641: ca        DEX
9642: d0 fb     BNE $963f
9644: 86 ea     STX $ea
9646: a6 92     LDX $92
9648: b5 9d     LDA $9d,X
964A: 4a        LSR A
964B: 05 a0     ORA $a0
964D: a4 a0     LDY $a0
964F: c0 04     CPY #$04
9651: d0 09     BNE $965c
9653: ad 0a d2  LDA $d20a
9656: 29 1f     AND #$1f
9658: c9 18     CMP #$18
965A: b0 f7     BCS $9653
965C: 0a        ASL A
965D: 0a        ASL A
965E: a4 d9     LDY $d9
9660: d0 17     BNE $9679
9662: 66 a0     ROR $a0
9664: b0 0e     BCS $9674
9666: 48        PHA
9667: b5 9d     LDA $9d,X
9669: 4a        LSR A
966A: 4a        LSR A
966B: 4a        LSR A
966C: aa        TAX
966D: bc 29 98  LDY $9829,X
9670: 84 ea     STY $ea
9672: 68        PLA
9673: 18        CLC
9674: 26 a0     ROL $a0
9676: 18        CLC
9677: 69 60     ADC #$60
9679: aa        TAX
967A: bd 5f 94  LDA $945f,X
967D: 29 7e     AND #$7e
967F: 85 c6     STA $c6
9681: a9 00     LDA #$00
9683: 5e 5f 94  LSR $945f,X
9686: 6a        ROR A
9687: 85 2c     STA $2c
9689: f0 02     BEQ $968d
968B: e6 99     INC $99
968D: bd 60 94  LDA $9460,X
9690: 29 7e     AND #$7e
9692: 85 c7     STA $c7
9694: a9 00     LDA #$00
9696: 5e 60 94  LSR $9460,X
9699: 6a        ROR A
969A: 85 2d     STA $2d
969C: f0 02     BEQ $96a0
969E: e6 99     INC $99
96A0: bd 61 94  LDA $9461,X
96A3: 85 c8     STA $c8
96A5: 29 3f     AND #$3f
96A7: a8        TAY
96A8: b9 1f 95  LDA $951f,Y
96AB: 85 c9     STA $c9
96AD: 85 ca     STA $ca
96AF: bd 62 94  LDA $9462,X
96B2: 4a        LSR A
96B3: 4a        LSR A
96B4: 4a        LSR A
96B5: 4a        LSR A
96B6: 4a        LSR A
96B7: 85 59     STA $59
96B9: a9 ff     LDA #$ff
96BB: a0 00     LDY #$00
96BD: 99 1a 00  STA $001a,Y
96C0: c8        INY
96C1: 38        SEC
96C2: e5 59     SBC $59
96C4: c0 0a     CPY #$0a
96C6: d0 f5     BNE $96bd
96C8: bd 62 94  LDA $9462,X
96CB: 29 1f     AND #$1f
96CD: 0a        ASL A
96CE: aa        TAX
96CF: bd 27 95  LDA $9527,X
96D2: 85 be     STA $be
96D4: bd 28 95  LDA $9528,X
96D7: 85 bf     STA $bf
96D9: a0 00     LDY #$00
96DB: 84 cb     STY $cb
96DD: b1 be     LDA ($be),Y
96DF: d0 32     BNE $9713
96E1: e6 a0     INC $a0
96E3: a6 cb     LDX $cb
96E5: a9 ff     LDA #$ff
96E7: 9d d6 08  STA $08d6,X
96EA: 9d 56 09  STA $0956,X
96ED: a9 3c     LDA #$3c
96EF: 85 a5     STA $a5
96F1: a9 03     LDA #$03
96F3: a6 92     LDX $92
96F5: b4 9d     LDY $9d,X
96F7: c0 28     CPY #$28
96F9: 90 02     BCC $96fd
96FB: a9 02     LDA #$02
96FD: 85 ef     STA $ef
96FF: 85 f0     STA $f0
9701: 85 f2     STA $f2
9703: 4a        LSR A
9704: 85 f1     STA $f1
9706: a9 02     LDA #$02
9708: a4 d9     LDY $d9
970A: d0 02     BNE $970e
970C: a9 03     LDA #$03
970E: 85 f9     STA $f9
9710: e6 9f     INC $9f
9712: 60        RTS
9713: 29 0f     AND #$0f
9715: 85 c0     STA $c0
9717: b1 be     LDA ($be),Y
9719: 4a        LSR A
971A: 4a        LSR A
971B: 4a        LSR A
971C: 4a        LSR A
971D: 85 c1     STA $c1
971F: c9 08     CMP #$08
9721: 90 12     BCC $9735
9723: a5 c0     LDA $c0
9725: 85 c3     STA $c3
9727: 85 c2     STA $c2
9729: a2 01     LDX #$01
972B: 86 c0     STX $c0
972D: ca        DEX
972E: 86 c5     STX $c5
9730: 86 c4     STX $c4
9732: 4c cf 97  JMP $97cf
9735: a6 c1     LDX $c1
9737: bd 0e 98  LDA $980e,X
973A: 18        CLC
973B: 65 c9     ADC $c9
973D: 85 c9     STA $c9
973F: bd 0e 98  LDA $980e,X
9742: 18        CLC
9743: 65 ca     ADC $ca
9745: 85 ca     STA $ca
9747: bd 17 98  LDA $9817,X
974A: 24 c8     BIT $c8
974C: 50 03     BVC $9751
974E: 20 08 98  JSR $9808
9751: 18        CLC
9752: 65 c7     ADC $c7
9754: 29 7f     AND #$7f
9756: 85 c7     STA $c7
9758: bd 17 98  LDA $9817,X
975B: 24 c8     BIT $c8
975D: 10 03     BPL $9762
975F: 20 08 98  JSR $9808
9762: 18        CLC
9763: 65 c6     ADC $c6
9765: 29 7f     AND #$7f
9767: 85 c6     STA $c6
9769: a5 ea     LDA $ea
976B: d0 0a     BNE $9777
976D: a5 c1     LDA $c1
976F: 4a        LSR A
9770: d0 11     BNE $9783
9772: 2a        ROL A
9773: 2a        ROL A
9774: 2a        ROL A
9775: 69 06     ADC #$06
9777: 85 c4     STA $c4
9779: 85 c5     STA $c5
977B: a9 00     LDA #$00
977D: 85 c2     STA $c2
977F: 85 c3     STA $c3
9781: f0 4c     BEQ $97cf
9783: a9 08     LDA #$08
9785: a6 c1     LDX $c1
9787: e0 05     CPX #$05
9789: b0 02     BCS $978d
978B: a9 0c     LDA #$0c
978D: 48        PHA
978E: 24 c8     BIT $c8
9790: 50 02     BVC $9794
9792: 49 04     EOR #$04
9794: 85 c5     STA $c5
9796: 68        PLA
9797: 24 c8     BIT $c8
9799: 10 02     BPL $979d
979B: 49 04     EOR #$04
979D: 85 c4     STA $c4
979F: a5 c7     LDA $c7
97A1: 4a        LSR A
97A2: 4a        LSR A
97A3: 4a        LSR A
97A4: 85 59     STA $59
97A6: bd 20 98  LDA $9820,X
97A9: 24 c8     BIT $c8
97AB: 50 03     BVC $97b0
97AD: 20 08 98  JSR $9808
97B0: 18        CLC
97B1: 65 59     ADC $59
97B3: 29 0f     AND #$0f
97B5: 85 c3     STA $c3
97B7: a5 c6     LDA $c6
97B9: 4a        LSR A
97BA: 4a        LSR A
97BB: 4a        LSR A
97BC: 85 59     STA $59
97BE: bd 20 98  LDA $9820,X
97C1: 24 c8     BIT $c8
97C3: 10 03     BPL $97c8
97C5: 20 08 98  JSR $9808
97C8: 18        CLC
97C9: 65 59     ADC $59
97CB: 29 0f     AND #$0f
97CD: 85 c2     STA $c2
97CF: a6 cb     LDX $cb
97D1: a5 c7     LDA $c7
97D3: 9d d6 07  STA $07d6,X
97D6: a5 c6     LDA $c6
97D8: 9d 56 08  STA $0856,X
97DB: a5 ca     LDA $ca
97DD: 9d d6 06  STA $06d6,X
97E0: a5 c9     LDA $c9
97E2: 9d 56 07  STA $0756,X
97E5: a5 c3     LDA $c3
97E7: 0a        ASL A
97E8: 0a        ASL A
97E9: 0a        ASL A
97EA: 0a        ASL A
97EB: 05 c5     ORA $c5
97ED: 9d d6 08  STA $08d6,X
97F0: a5 c2     LDA $c2
97F2: 0a        ASL A
97F3: 0a        ASL A
97F4: 0a        ASL A
97F5: 0a        ASL A
97F6: 05 c4     ORA $c4
97F8: 9d 56 09  STA $0956,X
97FB: e6 cb     INC $cb
97FD: c6 c0     DEC $c0
97FF: f0 03     BEQ $9804
9801: 4c 35 97  JMP $9735
9804: c8        INY
9805: 4c dd 96  JMP $96dd
9808: 49 ff     EOR #$ff
980A: 18        CLC
980B: 69 01     ADC #$01
980D: 60        RTS
982F: 86 58     STX $58
9831: a4 92     LDY $92
9833: be ba 98  LDX $98ba,Y
9836: 4a        LSR A
9837: 90 10     BCC $9849
9839: 48        PHA
983A: bd 10 21  LDA $2110,X
983D: 69 04     ADC #$04
983F: c9 0a     CMP #$0a
9841: 90 02     BCC $9845
9843: e9 0a     SBC #$0a
9845: 9d 10 21  STA $2110,X
9848: 68        PLA
9849: 7d 0f 21  ADC $210f,X
984C: c9 06     CMP #$06
984E: d0 1b     BNE $986b
9850: e0 03     CPX #$03
9852: d0 04     BNE $9858
9854: e6 aa     INC $aa
9856: d0 06     BNE $985e
9858: e0 09     CPX #$09
985A: d0 0f     BNE $986b
985C: e6 ab     INC $ab
985E: 48        PHA
985F: 8a        TXA
9860: 48        PHA
9861: 98        TYA
9862: 48        PHA
9863: 20 bc 98  JSR $98bc
9866: 68        PLA
9867: a8        TAY
9868: 68        PLA
9869: aa        TAX
986A: 68        PLA
986B: c9 0a     CMP #$0a
986D: 90 0a     BCC $9879
986F: e9 0a     SBC #$0a
9871: 9d 0f 21  STA $210f,X
9874: a9 00     LDA #$00
9876: ca        DEX
9877: d0 d0     BNE $9849
9879: 9d 0f 21  STA $210f,X
987C: be ba 98  LDX $98ba,Y
987F: a5 57     LDA $57
9881: d0 07     BNE $988a
9883: 98        TYA
9884: f0 06     BEQ $988c
9886: a0 0d     LDY #$0d
9888: d0 02     BNE $988c
988A: a0 04     LDY #$04
988C: a9 06     LDA #$06
988E: 85 50     STA $50
9890: a9 7f     LDA #$7f
9892: 85 4f     STA $4f
9894: bd 0b 21  LDA $210b,X
9897: d0 04     BNE $989d
9899: 24 4f     BIT $4f
989B: 10 07     BPL $98a4
989D: 09 d0     ORA #$d0
989F: 99 d8 3f  STA $3fd8,Y
98A2: 26 4f     ROL $4f
98A4: e8        INX
98A5: c8        INY
98A6: c6 50     DEC $50
98A8: d0 ea     BNE $9894
98AA: a5 57     LDA $57
98AC: d0 04     BNE $98b2
98AE: 24 4f     BIT $4f
98B0: 10 05     BPL $98b7
98B2: a9 d0     LDA #$d0
98B4: 99 d8 3f  STA $3fd8,Y
98B7: a6 58     LDX $58
98B9: 60        RTS
98BC: a6 92     LDX $92
98BE: b5 aa     LDA $aa,X
98C0: c9 05     CMP #$05
98C2: 90 02     BCC $98c6
98C4: a9 05     LDA #$05
98C6: 85 4e     STA $4e
98C8: a0 00     LDY #$00
98CA: a2 01     LDX #$01
98CC: a9 ff     LDA #$ff
98CE: e4 4e     CPX $4e
98D0: 90 02     BCC $98d4
98D2: a9 00     LDA #$00
98D4: 99 d8 3f  STA $3fd8,Y
98D7: c8        INY
98D8: e8        INX
98D9: e0 06     CPX #$06
98DB: d0 f1     BNE $98ce
98DD: 60        RTS
98DE: 86 fa     STX $fa
98E0: a0 00     LDY #$00
98E2: bd 8c 99  LDA $998c,X
98E5: 99 c4 3f  STA $3fc4,Y
98E8: e8        INX
98E9: c8        INY
98EA: c0 14     CPY #$14
98EC: d0 f4     BNE $98e2
98EE: e0 28     CPX #$28
98F0: f0 59     BEQ $994b
98F2: 90 2e     BCC $9922
98F4: e0 50     CPX #$50
98F6: f0 26     BEQ $991e
98F8: 90 59     BCC $9953
98FA: a9 28     LDA #$28
98FC: e5 da     SBC $da
98FE: 29 7f     AND #$7f
9900: a2 00     LDX #$00
9902: c9 0a     CMP #$0a
9904: 90 05     BCC $990b
9906: e8        INX
9907: e9 0a     SBC #$0a
9909: b0 f7     BCS $9902
990B: 09 d0     ORA #$d0
990D: 8d cc 3f  STA $3fcc
9910: 8d d0 3f  STA $3fd0
9913: 8a        TXA
9914: 09 d0     ORA #$d0
9916: 8d cb 3f  STA $3fcb
9919: 8d cf 3f  STA $3fcf
991C: 90 35     BCC $9953
991E: a2 06     LDX #$06
9920: d0 14     BNE $9936
9922: a6 92     LDX $92
9924: b5 9b     LDA $9b,X
9926: 09 d0     ORA #$d0
9928: 8d c5 3f  STA $3fc5
992B: c9 d1     CMP #$d1
992D: d0 05     BNE $9934
992F: a9 00     LDA #$00
9931: 8d cb 3f  STA $3fcb
9934: a2 0c     LDX #$0c
9936: a4 92     LDY $92
9938: b9 9d 00  LDA $009d,Y
993B: a8        TAY
993C: b9 5c 99  LDA $995c,Y
993F: 9d c4 3f  STA $3fc4,X
9942: e8        INX
9943: c8        INY
9944: 98        TYA
9945: 29 07     AND #$07
9947: d0 f3     BNE $993c
9949: f0 08     BEQ $9953
994B: a5 92     LDA $92
994D: 18        CLC
994E: 69 d1     ADC #$d1
9950: 8d d4 3f  STA $3fd4
9953: a9 96     LDA #$96
9955: 85 9a     STA $9a
9957: a2 09     LDX #$09
9959: 4c e3 8d  JMP $8de3
99F0: a6 92     LDX $92
99F2: b5 a3     LDA $a3,X
99F4: a2 12     LDX #$12
99F6: a0 ff     LDY #$ff
99F8: 84 4e     STY $4e
99FA: a0 0a     LDY #$0a
99FC: 84 4f     STY $4f
99FE: 20 05 9a  JSR $9a05
9A01: a0 01     LDY #$01
9A03: 84 4f     STY $4f
9A05: 38        SEC
9A06: a0 cf     LDY #$cf
9A08: c8        INY
9A09: e5 4f     SBC $4f
9A0B: b0 fb     BCS $9a08
9A0D: 65 4f     ADC $4f
9A0F: 48        PHA
9A10: c0 d0     CPY #$d0
9A12: d0 04     BNE $9a18
9A14: 24 4e     BIT $4e
9A16: 30 07     BMI $9a1f
9A18: e6 4e     INC $4e
9A1A: 98        TYA
9A1B: 9d d8 3f  STA $3fd8,X
9A1E: e8        INX
9A1F: 68        PLA
9A20: 60        RTS
9A21: ad 09 d2  LDA $d209
9A24: c9 21     CMP #$21
9A26: d0 13     BNE $9a3b
9A28: ad 0f d2  LDA $d20f
9A2B: 29 04     AND #$04
9A2D: d0 0c     BNE $9a3b
9A2F: a5 ee     LDA $ee
9A31: f0 0a     BEQ $9a3d
9A33: a5 93     LDA $93
9A35: 49 01     EOR #$01
9A37: 85 93     STA $93
9A39: a9 00     LDA #$00
9A3B: 85 ee     STA $ee
9A3D: ad 00 d3  LDA $d300
9A40: a6 92     LDX $92
9A42: f0 04     BEQ $9a48
9A44: 4a        LSR A
9A45: 4a        LSR A
9A46: 4a        LSR A
9A47: 4a        LSR A
9A48: 29 0f     AND #$0f
9A4A: 85 8f     STA $8f
9A4C: bc 10 d0  LDY $d010,X
9A4F: f0 05     BEQ $9a56
9A51: 88        DEY
9A52: 84 ed     STY $ed
9A54: f0 08     BEQ $9a5e
9A56: a6 ed     LDX $ed
9A58: d0 04     BNE $9a5e
9A5A: e6 90     INC $90
9A5C: e6 ed     INC $ed
9A5E: a9 08     LDA #$08
9A60: 8d 1f d0  STA $d01f
9A63: a9 03     LDA #$03
9A65: 8d 0f d2  STA $d20f
9A68: ad 1f d0  LDA $d01f
9A6B: 4a        LSR A
9A6C: 90 08     BCC $9a76
9A6E: a6 57     LDX $57
9A70: d0 0d     BNE $9a7f
9A72: a6 90     LDX $90
9A74: f0 09     BEQ $9a7f
9A76: a5 eb     LDA $eb
9A78: d0 19     BNE $9a93
9A7A: e6 eb     INC $eb
9A7C: e6 8d     INC $8d
9A7E: 60        RTS
9A7F: a2 00     LDX #$00
9A81: 86 eb     STX $eb
9A83: 4a        LSR A
9A84: b0 09     BCS $9a8f
9A86: a5 ec     LDA $ec
9A88: d0 09     BNE $9a93
9A8A: e6 ec     INC $ec
9A8C: e6 8e     INC $8e
9A8E: 60        RTS
9A8F: a9 00     LDA #$00
9A91: 85 ec     STA $ec
9A93: 60        RTS
9A94: 4c db b9  JMP $b9db
9A97: 20 5b 88  JSR $885b
9A9A: bd 1e 05  LDA $051e,X
9A9D: 85 8b     STA $8b
9A9F: bd 76 05  LDA $0576,X
9AA2: 85 8c     STA $8c
9AA4: 86 58     STX $58
9AA6: 20 cb a0  JSR $a0cb
9AA9: a6 58     LDX $58
9AAB: b0 08     BCS $9ab5
9AAD: c0 60     CPY #$60
9AAF: b0 04     BCS $9ab5
9AB1: c9 a0     CMP #$a0
9AB3: 90 03     BCC $9ab8
9AB5: 4c 91 9b  JMP $9b91
9AB8: 9d 6e 04  STA $046e,X
9ABB: 98        TYA
9ABC: 9d c6 04  STA $04c6,X
9ABF: a5 8c     LDA $8c
9AC1: 4a        LSR A
9AC2: 4a        LSR A
9AC3: 4a        LSR A
9AC4: 4a        LSR A
9AC5: 08        PHP
9AC6: 18        CLC
9AC7: 69 01     ADC #$01
9AC9: a8        TAY
9ACA: b1 46     LDA ($46),Y
9ACC: 28        PLP
9ACD: b0 04     BCS $9ad3
9ACF: 4a        LSR A
9AD0: 4a        LSR A
9AD1: 4a        LSR A
9AD2: 4a        LSR A
9AD3: 29 0f     AND #$0f
9AD5: 0a        ASL A
9AD6: 3e ce 05  ROL $05ce,X
9AD9: 6a        ROR A
9ADA: 9d ce 05  STA $05ce,X
9ADD: a0 00     LDY #$00
9ADF: b1 46     LDA ($46),Y
9AE1: 30 1a     BMI $9afd
9AE3: f0 0d     BEQ $9af2
9AE5: a8        TAY
9AE6: bd 1e 05  LDA $051e,X
9AE9: 4a        LSR A
9AEA: 4a        LSR A
9AEB: 4a        LSR A
9AEC: 88        DEY
9AED: f0 03     BEQ $9af2
9AEF: 4a        LSR A
9AF0: d0 fa     BNE $9aec
9AF2: 9d 26 06  STA $0626,X
9AF5: 4c fd 9a  JMP $9afd
9AF8: 20 5b 88  JSR $885b
9AFB: 86 58     STX $58
9AFD: bd ce 05  LDA $05ce,X
9B00: 29 7f     AND #$7f
9B02: f0 b1     BEQ $9ab5
9B04: 85 4f     STA $4f
9B06: 4a        LSR A
9B07: 85 4e     STA $4e
9B09: bd c6 04  LDA $04c6,X
9B0C: 38        SEC
9B0D: e5 4e     SBC $4e
9B0F: b0 02     BCS $9b13
9B11: a9 00     LDA #$00
9B13: 85 50     STA $50
9B15: a8        TAY
9B16: bd 6e 04  LDA $046e,X
9B19: 38        SEC
9B1A: e5 4e     SBC $4e
9B1C: b0 02     BCS $9b20
9B1E: a9 00     LDA #$00
9B20: 85 52     STA $52
9B22: 20 8a 87  JSR $878a
9B25: a6 58     LDX $58
9B27: a5 4e     LDA $4e
9B29: 29 02     AND #$02
9B2B: f0 02     BEQ $9b2f
9B2D: a9 3a     LDA #$3a
9B2F: 18        CLC
9B30: 7d 7e 06  ADC $067e,X
9B33: a8        TAY
9B34: a5 4e     LDA $4e
9B36: 29 01     AND #$01
9B38: 85 51     STA $51
9B3A: b9 cf ac  LDA $accf,Y
9B3D: 85 4a     STA $4a
9B3F: b9 d0 ac  LDA $acd0,Y
9B42: 85 4b     STA $4b
9B44: bd 26 06  LDA $0626,X
9B47: 4a        LSR A
9B48: 29 06     AND #$06
9B4A: a8        TAY
9B4B: b1 4a     LDA ($4a),Y
9B4D: 85 48     STA $48
9B4F: c8        INY
9B50: b1 4a     LDA ($4a),Y
9B52: 85 49     STA $49
9B54: bd 26 06  LDA $0626,X
9B57: 29 03     AND #$03
9B59: 18        CLC
9B5A: 69 05     ADC #$05
9B5C: a8        TAY
9B5D: b1 46     LDA ($46),Y
9B5F: 48        PHA
9B60: a5 4f     LDA $4f
9B62: 69 08     ADC #$08
9B64: a8        TAY
9B65: 68        PLA
9B66: 71 46     ADC ($46),Y
9B68: 90 03     BCC $9b6d
9B6A: e6 49     INC $49
9B6C: 18        CLC
9B6D: 65 48     ADC $48
9B6F: 85 48     STA $48
9B71: 90 02     BCC $9b75
9B73: e6 49     INC $49
9B75: a0 00     LDY #$00
9B77: a5 4f     LDA $4f
9B79: 18        CLC
9B7A: 69 06     ADC #$06
9B7C: 4a        LSR A
9B7D: 4a        LSR A
9B7E: 85 4e     STA $4e
9B80: a5 4f     LDA $4f
9B82: 18        CLC
9B83: 65 50     ADC $50
9B85: c9 60     CMP #$60
9B87: b0 08     BCS $9b91
9B89: a5 4f     LDA $4f
9B8B: 65 52     ADC $52
9B8D: c9 a0     CMP #$a0
9B8F: 90 07     BCC $9b98
9B91: a9 00     LDA #$00
9B93: 9d ce 05  STA $05ce,X
9B96: 38        SEC
9B97: 60        RTS
9B98: a5 51     LDA $51
9B9A: d0 35     BNE $9bd1
9B9C: a5 4e     LDA $4e
9B9E: 85 50     STA $50
9BA0: a2 00     LDX #$00
9BA2: b1 48     LDA ($48),Y
9BA4: 41 09     EOR ($09,X)
9BA6: 81 09     STA ($09,X)
9BA8: c8        INY
9BA9: d0 02     BNE $9bad
9BAB: e6 49     INC $49
9BAD: c6 50     DEC $50
9BAF: f0 08     BEQ $9bb9
9BB1: e6 09     INC $09
9BB3: d0 ed     BNE $9ba2
9BB5: e6 0a     INC $0a
9BB7: d0 e9     BNE $9ba2
9BB9: c6 4f     DEC $4f
9BBB: f0 10     BEQ $9bcd
9BBD: a6 4e     LDX $4e
9BBF: 18        CLC
9BC0: bd 0f 9c  LDA $9c0f,X
9BC3: 65 09     ADC $09
9BC5: 85 09     STA $09
9BC7: 90 cf     BCC $9b98
9BC9: e6 0a     INC $0a
9BCB: b0 cb     BCS $9b98
9BCD: a6 58     LDX $58
9BCF: 18        CLC
9BD0: 60        RTS
9BD1: a5 4e     LDA $4e
9BD3: 85 50     STA $50
9BD5: a2 00     LDX #$00
9BD7: 86 51     STX $51
9BD9: b1 48     LDA ($48),Y
9BDB: 66 51     ROR $51
9BDD: 6a        ROR A
9BDE: 08        PHP
9BDF: 66 51     ROR $51
9BE1: 6a        ROR A
9BE2: 41 09     EOR ($09,X)
9BE4: 81 09     STA ($09,X)
9BE6: 26 51     ROL $51
9BE8: 28        PLP
9BE9: 26 51     ROL $51
9BEB: c8        INY
9BEC: d0 02     BNE $9bf0
9BEE: e6 49     INC $49
9BF0: c6 50     DEC $50
9BF2: f0 08     BEQ $9bfc
9BF4: e6 09     INC $09
9BF6: d0 e1     BNE $9bd9
9BF8: e6 0a     INC $0a
9BFA: d0 dd     BNE $9bd9
9BFC: c6 4f     DEC $4f
9BFE: f0 cd     BEQ $9bcd
9C00: a6 4e     LDX $4e
9C02: 18        CLC
9C03: bd 0f 9c  LDA $9c0f,X
9C06: 65 09     ADC $09
9C08: 85 09     STA $09
9C0A: 90 c5     BCC $9bd1
9C0C: e6 0a     INC $0a
9C0E: b0 c1     BCS $9bd1
9C10: 28        PLP
9C11: 27        ???
9C12: 26 25     ROL $25
9C14: ad 1e 05  LDA $051e
9C17: 85 8b     STA $8b
9C19: ad 76 05  LDA $0576
9C1C: c9 3c     CMP #$3c
9C1E: b0 b0     BCS $9bd0
9C20: 85 8c     STA $8c
9C22: 20 cb a0  JSR $a0cb
9C25: e9 04     SBC #$04
9C27: 2c 1e 05  BIT $051e
9C2A: 70 02     BVS $9c2e
9C2C: 69 02     ADC #$02
9C2E: 8d 6e 04  STA $046e
9C31: ad 1e 05  LDA $051e
9C34: 69 04     ADC #$04
9C36: 29 7e     AND #$7e
9C38: 85 4f     STA $4f
9C3A: 4a        LSR A
9C3B: 4a        LSR A
9C3C: 29 07     AND #$07
9C3E: 06 4f     ASL $4f
9C40: 24 4f     BIT $4f
9C42: 30 07     BMI $9c4b
9C44: 50 09     BVC $9c4f
9C46: e9 07     SBC #$07
9C48: 4c 53 9c  JMP $9c53
9C4B: 50 06     BVC $9c53
9C4D: e9 07     SBC #$07
9C4F: 49 ff     EOR #$ff
9C51: 69 01     ADC #$01
9C53: 46 4f     LSR $4f
9C55: 6d 6e 04  ADC $046e
9C58: 8d 6e 04  STA $046e
9C5B: 8c c6 04  STY $04c6
9C5E: 18        CLC
9C5F: 69 2c     ADC #$2c
9C61: 8d 00 d0  STA $d000
9C64: ae 76 05  LDX $0576
9C67: e0 14     CPX #$14
9C69: 90 0c     BCC $9c77
9C6B: 8d 01 d0  STA $d001
9C6E: a9 44     LDA #$44
9C70: 8d 13 d0  STA $d013
9C73: a9 00     LDA #$00
9C75: f0 1a     BEQ $9c91
9C77: 69 08     ADC #$08
9C79: 24 4f     BIT $4f
9C7B: 50 02     BVC $9c7f
9C7D: e9 0f     SBC #$0f
9C7F: 8d 01 d0  STA $d001
9C82: a5 4f     LDA $4f
9C84: 4a        LSR A
9C85: 4a        LSR A
9C86: 4a        LSR A
9C87: aa        TAX
9C88: ad 6e 04  LDA $046e
9C8B: 18        CLC
9C8C: 7d 2b b8  ADC $b82b,X
9C8F: 69 2c     ADC #$2c
9C91: 8d 02 d0  STA $d002
9C94: ad 76 05  LDA $0576
9C97: 38        SEC
9C98: e9 0c     SBC #$0c
9C9A: 4a        LSR A
9C9B: 4a        LSR A
9C9C: 29 0e     AND #$0e
9C9E: aa        TAX
9C9F: f0 06     BEQ $9ca7
9CA1: c9 08     CMP #$08
9CA3: 90 02     BCC $9ca7
9CA5: a2 06     LDX #$06
9CA7: 24 4f     BIT $4f
9CA9: 50 17     BVC $9cc2
9CAB: 48        PHA
9CAC: 8a        TXA
9CAD: 18        CLC
9CAE: 69 08     ADC #$08
9CB0: aa        TAX
9CB1: 68        PLA
9CB2: d0 0e     BNE $9cc2
9CB4: ad 6e 04  LDA $046e
9CB7: 18        CLC
9CB8: 69 2c     ADC #$2c
9CBA: 8d 01 d0  STA $d001
9CBD: 69 08     ADC #$08
9CBF: 8d 00 d0  STA $d000
9CC2: bd a4 9d  LDA $9da4,X
9CC5: 85 0a     STA $0a
9CC7: bd a3 9d  LDA $9da3,X
9CCA: 85 09     STA $09
9CCC: 18        CLC
9CCD: 7d b3 9d  ADC $9db3,X
9CD0: 85 46     STA $46
9CD2: a5 0a     LDA $0a
9CD4: 69 00     ADC #$00
9CD6: 85 47     STA $47
9CD8: a5 46     LDA $46
9CDA: 7d b3 9d  ADC $9db3,X
9CDD: 85 48     STA $48
9CDF: a5 47     LDA $47
9CE1: 69 00     ADC #$00
9CE3: 85 49     STA $49
9CE5: bd b3 9d  LDA $9db3,X
9CE8: 85 4e     STA $4e
9CEA: a5 4f     LDA $4f
9CEC: 4a        LSR A
9CED: 4a        LSR A
9CEE: 4a        LSR A
9CEF: 48        PHA
9CF0: 29 07     AND #$07
9CF2: c9 05     CMP #$05
9CF4: 90 06     BCC $9cfc
9CF6: 49 03     EOR #$03
9CF8: 69 00     ADC #$00
9CFA: 29 03     AND #$03
9CFC: a8        TAY
9CFD: b1 09     LDA ($09),Y
9CFF: a8        TAY
9D00: 68        PLA
9D01: c9 05     CMP #$05
9D03: 90 24     BCC $9d29
9D05: c9 0d     CMP #$0d
9D07: b0 20     BCS $9d29
9D09: ad c6 04  LDA $04c6
9D0C: 69 0b     ADC #$0b
9D0E: 38        SEC
9D0F: fd b4 9d  SBC $9db4,X
9D12: aa        TAX
9D13: ca        DEX
9D14: b1 09     LDA ($09),Y
9D16: 9d 10 22  STA $2210,X
9D19: b1 46     LDA ($46),Y
9D1B: 9d 90 22  STA $2290,X
9D1E: b1 48     LDA ($48),Y
9D20: 9d 10 23  STA $2310,X
9D23: c8        INY
9D24: c6 4e     DEC $4e
9D26: d0 eb     BNE $9d13
9D28: 60        RTS
9D29: ad c6 04  LDA $04c6
9D2C: 18        CLC
9D2D: 7d b4 9d  ADC $9db4,X
9D30: aa        TAX
9D31: b1 09     LDA ($09),Y
9D33: 9d 10 22  STA $2210,X
9D36: b1 46     LDA ($46),Y
9D38: 9d 90 22  STA $2290,X
9D3B: b1 48     LDA ($48),Y
9D3D: 9d 10 23  STA $2310,X
9D40: e8        INX
9D41: c8        INY
9D42: c6 4e     DEC $4e
9D44: d0 eb     BNE $9d31
9D46: 60        RTS
9D5B: a0 0f     LDY #$0f
9D5D: b9 47 9d  LDA $9d47,Y
9D60: 85 09     STA $09
9D62: b9 48 9d  LDA $9d48,Y
9D65: 85 0a     STA $0a
9D67: b9 49 9d  LDA $9d49,Y
9D6A: 85 46     STA $46
9D6C: b9 4a 9d  LDA $9d4a,Y
9D6F: 85 47     STA $47
9D71: be 4b 9d  LDX $9d4b,Y
9D74: 98        TYA
9D75: 48        PHA
9D76: 20 81 9d  JSR $9d81
9D79: 68        PLA
9D7A: 38        SEC
9D7B: e9 05     SBC #$05
9D7D: a8        TAY
9D7E: 10 dd     BPL $9d5d
9D80: 60        RTS
9D81: 86 4e     STX $4e
9D83: a0 00     LDY #$00
9D85: a2 05     LDX #$05
9D87: b1 09     LDA ($09),Y
9D89: 91 46     STA ($46),Y
9D8B: c8        INY
9D8C: ca        DEX
9D8D: d0 f8     BNE $9d87
9D8F: b1 09     LDA ($09),Y
9D91: 85 4f     STA $4f
9D93: a2 08     LDX #$08
9D95: 26 4f     ROL $4f
9D97: 6a        ROR A
9D98: ca        DEX
9D99: d0 fa     BNE $9d95
9D9B: 91 46     STA ($46),Y
9D9D: c8        INY
9D9E: c6 4e     DEC $4e
9DA0: d0 ed     BNE $9d8f
9DA2: 60        RTS
9F38: a5 57     LDA $57
9F3A: d0 04     BNE $9f40
9F3C: a2 28     LDX #$28
9F3E: d0 04     BNE $9f44
9F40: a4 92     LDY $92
9F42: b6 9d     LDX $9d,Y
9F44: bd 08 9f  LDA $9f08,X
9F47: 85 46     STA $46
9F49: bd 09 9f  LDA $9f09,X
9F4C: 85 47     STA $47
9F4E: a0 00     LDY #$00
9F50: bd 0a 9f  LDA $9f0a,X
9F53: 99 16 d0  STA $d016,Y
9F56: e8        INX
9F57: c8        INY
9F58: c0 03     CPY #$03
9F5A: d0 f4     BNE $9f50
9F5C: bd 0a 9f  LDA $9f0a,X
9F5F: 29 07     AND #$07
9F61: 85 50     STA $50
9F63: bd 0a 9f  LDA $9f0a,X
9F66: 4a        LSR A
9F67: 4a        LSR A
9F68: 4a        LSR A
9F69: 85 4f     STA $4f
9F6B: bc 0b 9f  LDY $9f0b,X
9F6E: a5 57     LDA $57
9F70: d0 02     BNE $9f74
9F72: a0 30     LDY #$30
9F74: 84 51     STY $51
9F76: bd 0c 9f  LDA $9f0c,X
9F79: 85 52     STA $52
9F7B: 20 8a 87  JSR $878a
9F7E: a9 04     LDA #$04
9F80: 85 53     STA $53
9F82: a6 50     LDX $50
9F84: a9 00     LDA #$00
9F86: 85 4e     STA $4e
9F88: a4 53     LDY $53
9F8A: b1 46     LDA ($46),Y
9F8C: a4 4e     LDY $4e
9F8E: 51 09     EOR ($09),Y
9F90: 91 09     STA ($09),Y
9F92: e6 53     INC $53
9F94: e6 4e     INC $4e
9F96: ca        DEX
9F97: d0 ef     BNE $9f88
9F99: a5 09     LDA $09
9F9B: 18        CLC
9F9C: 69 28     ADC #$28
9F9E: 85 09     STA $09
9FA0: 90 02     BCC $9fa4
9FA2: e6 0a     INC $0a
9FA4: c6 4f     DEC $4f
9FA6: d0 da     BNE $9f82
9FA8: a0 00     LDY #$00
9FAA: a2 03     LDX #$03
9FAC: 20 d8 9f  JSR $9fd8
9FAF: b0 26     BCS $9fd7
9FB1: a0 02     LDY #$02
9FB3: b1 09     LDA ($09),Y
9FB5: 65 52     ADC $52
9FB7: 69 30     ADC #$30
9FB9: 8d 03 d0  STA $d003
9FBC: a0 02     LDY #$02
9FBE: a2 07     LDX #$07
9FC0: 20 d8 9f  JSR $9fd8
9FC3: b0 12     BCS $9fd7
9FC5: a0 02     LDY #$02
9FC7: b1 09     LDA ($09),Y
9FC9: 65 52     ADC $52
9FCB: 69 30     ADC #$30
9FCD: a2 04     LDX #$04
9FCF: 9d 03 d0  STA $d003,X
9FD2: 69 02     ADC #$02
9FD4: ca        DEX
9FD5: d0 f8     BNE $9fcf
9FD7: 60        RTS
9FD8: b1 46     LDA ($46),Y
9FDA: 85 09     STA $09
9FDC: b9 19 a0  LDA $a019,Y
9FDF: 85 48     STA $48
9FE1: c8        INY
9FE2: b1 46     LDA ($46),Y
9FE4: 85 0a     STA $0a
9FE6: b9 19 a0  LDA $a019,Y
9FE9: 85 49     STA $49
9FEB: a0 00     LDY #$00
9FED: b1 09     LDA ($09),Y
9FEF: d0 02     BNE $9ff3
9FF1: 38        SEC
9FF2: 60        RTS
9FF3: 85 4e     STA $4e
9FF5: c8        INY
9FF6: b1 09     LDA ($09),Y
9FF8: 9d 12 d0  STA $d012,X
9FFB: c8        INY
9FFC: c8        INY
9FFD: b1 09     LDA ($09),Y
9FFF: 18        CLC
A000: 65 51     ADC $51
A002: 85 50     STA $50
A004: c8        INY
A005: 84 4f     STY $4f
A007: a4 4f     LDY $4f
A009: b1 09     LDA ($09),Y
A00B: a4 50     LDY $50
A00D: 91 48     STA ($48),Y
A00F: e6 4f     INC $4f
A011: e6 50     INC $50
A013: c6 4e     DEC $4e
A015: d0 f0     BNE $a007
A017: 18        CLC
A018: 60        RTS
A01D: 84 4e     STY $4e
A01F: ad 1e 05  LDA $051e
A022: 4a        LSR A
A023: 4a        LSR A
A024: 4a        LSR A
A025: 18        CLC
A026: 69 01     ADC #$01
A028: 4a        LSR A
A029: 29 07     AND #$07
A02B: c9 04     CMP #$04
A02D: 90 09     BCC $a038
A02F: 48        PHA
A030: 98        TYA
A031: 09 40     ORA #$40
A033: 85 4e     STA $4e
A035: 68        PLA
A036: 29 03     AND #$03
A038: 85 4f     STA $4f
A03A: 0a        ASL A
A03B: 0a        ASL A
A03C: 65 4f     ADC $4f
A03E: a8        TAY
A03F: a2 05     LDX #$05
A041: b9 95 a0  LDA $a095,Y
A044: 20 7b a0  JSR $a07b
A047: 95 cd     STA $cd,X
A049: 20 87 a0  JSR $a087
A04C: 18        CLC
A04D: 6d 6e 04  ADC $046e
A050: 9d 8a 04  STA $048a,X
A053: b9 a9 a0  LDA $a0a9,Y
A056: 20 7b a0  JSR $a07b
A059: 95 d2     STA $d2,X
A05B: 20 87 a0  JSR $a087
A05E: 18        CLC
A05F: 6d c6 04  ADC $04c6
A062: 9d e2 04  STA $04e2,X
A065: a9 00     LDA #$00
A067: 9d 9a 06  STA $069a,X
A06A: 9d 42 06  STA $0642,X
A06D: a9 03     LDA #$03
A06F: 9d ea 05  STA $05ea,X
A072: c8        INY
A073: ca        DEX
A074: d0 cb     BNE $a041
A076: a9 88     LDA #$88
A078: 85 d8     STA $d8
A07A: 60        RTS
A07B: 24 4e     BIT $4e
A07D: 30 03     BMI $a082
A07F: 50 03     BVC $a084
A081: 60        RTS
A082: 50 fd     BVC $a081
A084: 4c 08 98  JMP $9808
A087: 24 4e     BIT $4e
A089: 30 07     BMI $a092
A08B: 20 08 98  JSR $9808
A08E: 0a        ASL A
A08F: 0a        ASL A
A090: 0a        ASL A
A091: 60        RTS
A092: a9 00     LDA #$00
A094: 60        RTS
A0BD: 38        SEC
A0BE: e5 4e     SBC $4e
A0C0: f0 08     BEQ $a0ca
A0C2: 0a        ASL A
A0C3: 30 03     BMI $a0c8
A0C5: a9 fe     LDA #$fe
A0C7: 60        RTS
A0C8: a9 02     LDA #$02
A0CA: 60        RTS
A0CB: a5 8b     LDA $8b
A0CD: 4a        LSR A
A0CE: a8        TAY
A0CF: 4a        LSR A
A0D0: 4a        LSR A
A0D1: 4a        LSR A
A0D2: 4a        LSR A
A0D3: aa        TAX
A0D4: b9 59 a5  LDA $a559,Y
A0D7: a8        TAY
A0D8: b9 99 a5  LDA $a599,Y
A0DB: 85 09     STA $09
A0DD: b9 9a a5  LDA $a59a,Y
A0E0: 85 0a     STA $0a
A0E2: a5 8c     LDA $8c
A0E4: 29 fe     AND #$fe
A0E6: a8        TAY
A0E7: b1 09     LDA ($09),Y
A0E9: 30 0f     BMI $a0fa
A0EB: 18        CLC
A0EC: 5d 14 a1  EOR $a114,X
A0EF: 10 01     BPL $a0f2
A0F1: 38        SEC
A0F2: 69 30     ADC #$30
A0F4: c9 59     CMP #$59
A0F6: 90 02     BCC $a0fa
A0F8: a9 ff     LDA #$ff
A0FA: 48        PHA
A0FB: c8        INY
A0FC: b1 09     LDA ($09),Y
A0FE: 30 09     BMI $a109
A100: 18        CLC
A101: 5d 15 a1  EOR $a115,X
A104: 10 01     BPL $a107
A106: 38        SEC
A107: 69 50     ADC #$50
A109: aa        TAX
A10A: 68        PLA
A10B: a8        TAY
A10C: 8a        TXA
A10D: c9 ff     CMP #$ff
A10F: b0 02     BCS $a113
A111: c0 ff     CPY #$ff
A113: 60        RTS
A5C7: a2 08     LDX #$08
A5C9: bd bb a5  LDA $a5bb,X
A5CC: 85 09     STA $09
A5CE: bd bc a5  LDA $a5bc,X
A5D1: 85 0a     STA $0a
A5D3: bd bd a5  LDA $a5bd,X
A5D6: 85 46     STA $46
A5D8: bd be a5  LDA $a5be,X
A5DB: 85 47     STA $47
A5DD: 86 4e     STX $4e
A5DF: a9 04     LDA #$04
A5E1: 85 53     STA $53
A5E3: a0 00     LDY #$00
A5E5: a2 03     LDX #$03
A5E7: a9 02     LDA #$02
A5E9: 20 21 a6  JSR $a621
A5EC: a0 06     LDY #$06
A5EE: a2 06     LDX #$06
A5F0: a9 03     LDA #$03
A5F2: 20 21 a6  JSR $a621
A5F5: a0 18     LDY #$18
A5F7: a2 07     LDX #$07
A5F9: a9 03     LDA #$03
A5FB: 20 21 a6  JSR $a621
A5FE: a5 09     LDA $09
A600: 18        CLC
A601: 69 2d     ADC #$2d
A603: 85 09     STA $09
A605: 90 02     BCC $a609
A607: e6 0a     INC $0a
A609: a5 46     LDA $46
A60B: 38        SEC
A60C: e9 2d     SBC #$2d
A60E: 85 46     STA $46
A610: b0 02     BCS $a614
A612: c6 47     DEC $47
A614: c6 53     DEC $53
A616: d0 cb     BNE $a5e3
A618: a6 4e     LDX $4e
A61A: ca        DEX
A61B: ca        DEX
A61C: ca        DEX
A61D: ca        DEX
A61E: 10 a9     BPL $a5c9
A620: 60        RTS
A621: 85 4f     STA $4f
A623: 86 50     STX $50
A625: 86 51     STX $51
A627: 84 52     STY $52
A629: a6 4f     LDX $4f
A62B: b1 09     LDA ($09),Y
A62D: 9d 1d 05  STA $051d,X
A630: c8        INY
A631: ca        DEX
A632: d0 f7     BNE $a62b
A634: bd 1e 05  LDA $051e,X
A637: 48        PHA
A638: e8        INX
A639: e4 4f     CPX $4f
A63B: d0 f7     BNE $a634
A63D: c6 50     DEC $50
A63F: d0 e8     BNE $a629
A641: a4 52     LDY $52
A643: a6 4f     LDX $4f
A645: 68        PLA
A646: 91 46     STA ($46),Y
A648: c8        INY
A649: ca        DEX
A64A: d0 f9     BNE $a645
A64C: c6 51     DEC $51
A64E: d0 f3     BNE $a643
A650: 60        RTS
A699: a2 00     LDX #$00
A69B: bd 51 a6  LDA $a651,X
A69E: 85 09     STA $09
A6A0: bd 52 a6  LDA $a652,X
A6A3: 85 0a     STA $0a
A6A5: bd 53 a6  LDA $a653,X
A6A8: 85 46     STA $46
A6AA: bd 54 a6  LDA $a654,X
A6AD: 85 47     STA $47
A6AF: e8        INX
A6B0: e8        INX
A6B1: e8        INX
A6B2: e8        INX
A6B3: 86 50     STX $50
A6B5: a9 04     LDA #$04
A6B7: 85 4e     STA $4e
A6B9: a9 04     LDA #$04
A6BB: 85 4f     STA $4f
A6BD: a6 50     LDX $50
A6BF: e6 50     INC $50
A6C1: bd 51 a6  LDA $a651,X
A6C4: f0 78     BEQ $a73e
A6C6: 4a        LSR A
A6C7: 4a        LSR A
A6C8: 4a        LSR A
A6C9: 4a        LSR A
A6CA: 85 48     STA $48
A6CC: 18        CLC
A6CD: 69 06     ADC #$06
A6CF: 4a        LSR A
A6D0: 4a        LSR A
A6D1: 85 4a     STA $4a
A6D3: bd 51 a6  LDA $a651,X
A6D6: 29 0f     AND #$0f
A6D8: a8        TAY
A6D9: 84 49     STY $49
A6DB: a9 00     LDA #$00
A6DD: 85 51     STA $51
A6DF: 18        CLC
A6E0: 65 4a     ADC $4a
A6E2: 88        DEY
A6E3: d0 fb     BNE $a6e0
A6E5: 85 52     STA $52
A6E7: a6 4a     LDX $4a
A6E9: a4 51     LDY $51
A6EB: b1 09     LDA ($09),Y
A6ED: 20 60 a7  JSR $a760
A6F0: 48        PHA
A6F1: c8        INY
A6F2: ca        DEX
A6F3: d0 f6     BNE $a6eb
A6F5: 84 51     STY $51
A6F7: a5 52     LDA $52
A6F9: 38        SEC
A6FA: e5 4a     SBC $4a
A6FC: a8        TAY
A6FD: a6 4a     LDX $4a
A6FF: 68        PLA
A700: 91 46     STA ($46),Y
A702: c8        INY
A703: ca        DEX
A704: d0 f9     BNE $a6ff
A706: a5 48     LDA $48
A708: 29 03     AND #$03
A70A: a8        TAY
A70B: be 5c a7  LDX $a75c,Y
A70E: a4 52     LDY $52
A710: a5 4a     LDA $4a
A712: 85 4b     STA $4b
A714: 18        CLC
A715: 88        DEY
A716: b1 46     LDA ($46),Y
A718: 2a        ROL A
A719: 91 46     STA ($46),Y
A71B: c6 4b     DEC $4b
A71D: d0 f6     BNE $a715
A71F: ca        DEX
A720: d0 ec     BNE $a70e
A722: 84 52     STY $52
A724: c6 49     DEC $49
A726: d0 bf     BNE $a6e7
A728: a5 51     LDA $51
A72A: 18        CLC
A72B: 65 09     ADC $09
A72D: 85 09     STA $09
A72F: 90 02     BCC $a733
A731: e6 0a     INC $0a
A733: a5 51     LDA $51
A735: 18        CLC
A736: 65 46     ADC $46
A738: 85 46     STA $46
A73A: 90 02     BCC $a73e
A73C: e6 47     INC $47
A73E: c6 4f     DEC $4f
A740: d0 0e     BNE $a750
A742: a5 50     LDA $50
A744: c6 4e     DEC $4e
A746: f0 0b     BEQ $a753
A748: 38        SEC
A749: e9 04     SBC #$04
A74B: 85 50     STA $50
A74D: 4c b9 a6  JMP $a6b9
A750: 4c bd a6  JMP $a6bd
A753: aa        TAX
A754: c9 48     CMP #$48
A756: f0 03     BEQ $a75b
A758: 4c 9b a6  JMP $a69b
A75B: 60        RTS
A760: 85 53     STA $53
A762: a9 04     LDA #$04
A764: 85 54     STA $54
A766: 26 53     ROL $53
A768: 08        PHP
A769: 26 53     ROL $53
A76B: 6a        ROR A
A76C: 28        PLP
A76D: 6a        ROR A
A76E: c6 54     DEC $54
A770: d0 f4     BNE $a766
A772: 60        RTS
A868: a2 00     LDX #$00
A86A: bd 73 a7  LDA $a773,X
A86D: 4a        LSR A
A86E: 4a        LSR A
A86F: 4a        LSR A
A870: 4a        LSR A
A871: 85 4a     STA $4a
A873: bd 73 a7  LDA $a773,X
A876: 29 0f     AND #$0f
A878: 85 59     STA $59
A87A: bd 74 a7  LDA $a774,X
A87D: 85 5a     STA $5a
A87F: bd 75 a7  LDA $a775,X
A882: 85 4b     STA $4b
A884: e8        INX
A885: e8        INX
A886: e8        INX
A887: bd 73 a7  LDA $a773,X
A88A: 85 09     STA $09
A88C: bd 74 a7  LDA $a774,X
A88F: 85 0a     STA $0a
A891: bd 75 a7  LDA $a775,X
A894: 85 46     STA $46
A896: bd 76 a7  LDA $a776,X
A899: 85 47     STA $47
A89B: 86 4f     STX $4f
A89D: a5 59     LDA $59
A89F: f0 0b     BEQ $a8ac
A8A1: 85 50     STA $50
A8A3: a2 02     LDX #$02
A8A5: 20 d9 a8  JSR $a8d9
A8A8: c6 50     DEC $50
A8AA: d0 f7     BNE $a8a3
A8AC: a5 5a     LDA $5a
A8AE: f0 0b     BEQ $a8bb
A8B0: 85 50     STA $50
A8B2: a2 03     LDX #$03
A8B4: 20 d9 a8  JSR $a8d9
A8B7: c6 50     DEC $50
A8B9: d0 f7     BNE $a8b2
A8BB: a5 4b     LDA $4b
A8BD: f0 0b     BEQ $a8ca
A8BF: 85 50     STA $50
A8C1: a2 04     LDX #$04
A8C3: 20 d9 a8  JSR $a8d9
A8C6: c6 50     DEC $50
A8C8: d0 f7     BNE $a8c1
A8CA: c6 4a     DEC $4a
A8CC: d0 cf     BNE $a89d
A8CE: a6 4f     LDX $4f
A8D0: e8        INX
A8D1: e8        INX
A8D2: e8        INX
A8D3: e8        INX
A8D4: e0 f5     CPX #$f5
A8D6: d0 92     BNE $a86a
A8D8: 60        RTS
A8D9: a9 00     LDA #$00
A8DB: a8        TAY
A8DC: 85 52     STA $52
A8DE: a9 04     LDA #$04
A8E0: 85 4e     STA $4e
A8E2: b1 09     LDA ($09),Y
A8E4: 4a        LSR A
A8E5: 66 52     ROR $52
A8E7: 90 02     BCC $a8eb
A8E9: 09 80     ORA #$80
A8EB: c6 4e     DEC $4e
A8ED: d0 f5     BNE $a8e4
A8EF: 46 52     LSR $52
A8F1: 46 52     LSR $52
A8F3: 46 52     LSR $52
A8F5: 46 52     LSR $52
A8F7: 91 46     STA ($46),Y
A8F9: e6 09     INC $09
A8FB: d0 02     BNE $a8ff
A8FD: e6 0a     INC $0a
A8FF: e6 46     INC $46
A901: d0 02     BNE $a905
A903: e6 47     INC $47
A905: ca        DEX
A906: d0 d6     BNE $a8de
A908: 60        RTS
B901: a8        TAY
B902: 8a        TXA
B903: 48        PHA
B904: 98        TYA
B905: c9 07     CMP #$07
B907: f0 06     BEQ $b90f
B909: 85 69     STA $69
B90B: a0 02     LDY #$02
B90D: 84 68     STY $68
B90F: 0a        ASL A
B910: 0a        ASL A
B911: 0a        ASL A
B912: aa        TAX
B913: a0 00     LDY #$00
B915: bd 55 b9  LDA $b955,X
B918: 99 5c 00  STA $005c,Y
B91B: e8        INX
B91C: c8        INY
B91D: c0 08     CPY #$08
B91F: 90 f4     BCC $b915
B921: a2 06     LDX #$06
B923: a0 01     LDY #$01
B925: a9 a3     LDA #$a3
B927: 95 6b     STA $6b,X
B929: a9 00     LDA #$00
B92B: 95 73     STA $73,X
B92D: 95 83     STA $83,X
B92F: 94 6a     STY $6a,X
B931: ca        DEX
B932: ca        DEX
B933: 10 f0     BPL $b925
B935: a6 69     LDX $69
B937: d0 04     BNE $b93d
B939: a0 06     LDY #$06
B93B: d0 05     BNE $b942
B93D: ca        DEX
B93E: d0 05     BNE $b945
B940: a0 04     LDY #$04
B942: 20 95 b9  JSR $b995
B945: a9 05     LDA #$05
B947: 85 67     STA $67
B949: 85 66     STA $66
B94B: 68        PLA
B94C: aa        TAX
B94D: 60        RTS
B995: 8a        TXA
B996: 48        PHA
B997: be b3 b9  LDX $b9b3,Y
B99A: 20 a0 b9  JSR $b9a0
B99D: 68        PLA
B99E: aa        TAX
B99F: 60        RTS
B9A0: b9 bd b9  LDA $b9bd,Y
B9A3: 95 7a     STA $7a,X
B9A5: b9 c7 b9  LDA $b9c7,Y
B9A8: 95 7b     STA $7b,X
B9AA: b9 d1 b9  LDA $b9d1,Y
B9AD: 95 82     STA $82,X
B9AF: c8        INY
B9B0: 94 72     STY $72,X
B9B2: 60        RTS
B9DB: a9 00     LDA #$00
B9DD: 8d 0e d4  STA $d40e
B9E0: a5 93     LDA $93
B9E2: f0 03     BEQ $b9e7
B9E4: 4c 18 bb  JMP $bb18
B9E7: a5 66     LDA $66
B9E9: d0 03     BNE $b9ee
B9EB: 4c 03 bb  JMP $bb03
B9EE: a2 06     LDX #$06
B9F0: a5 69     LDA $69
B9F2: c9 03     CMP #$03
B9F4: 90 09     BCC $b9ff
B9F6: a5 78     LDA $78
B9F8: d0 1b     BNE $ba15
B9FA: 8d 07 d2  STA $d207
B9FD: f0 16     BEQ $ba15
B9FF: b5 6b     LDA $6b,X
BA01: c9 a3     CMP #$a3
BA03: f0 02     BEQ $ba07
BA05: d6 6b     DEC $6b,X
BA07: b5 72     LDA $72,X
BA09: d0 0a     BNE $ba15
BA0B: b5 6b     LDA $6b,X
BA0D: 9d 01 d2  STA $d201,X
BA10: b5 83     LDA $83,X
BA12: 9d 00 d2  STA $d200,X
BA15: ca        DEX
BA16: ca        DEX
BA17: 10 e6     BPL $b9ff
BA19: c6 67     DEC $67
BA1B: d0 ce     BNE $b9eb
BA1D: a9 05     LDA #$05
BA1F: 85 67     STA $67
BA21: a2 06     LDX #$06
BA23: a5 69     LDA $69
BA25: c9 03     CMP #$03
BA27: 90 67     BCC $ba90
BA29: c6 70     DEC $70
BA2B: d0 48     BNE $ba75
BA2D: a9 04     LDA #$04
BA2F: 85 70     STA $70
BA31: a1 5c     LDA ($5c,X)
BA33: 85 71     STA $71
BA35: e6 62     INC $62
BA37: d0 02     BNE $ba3b
BA39: e6 63     INC $63
BA3B: a8        TAY
BA3C: d0 37     BNE $ba75
BA3E: a1 5c     LDA ($5c,X)
BA40: e6 62     INC $62
BA42: d0 02     BNE $ba46
BA44: e6 63     INC $63
BA46: a8        TAY
BA47: d0 0a     BNE $ba53
BA49: a9 aa     LDA #$aa
BA4B: 85 62     STA $62
BA4D: a9 bf     LDA #$bf
BA4F: 85 63     STA $63
BA51: d0 de     BNE $ba31
BA53: 30 06     BMI $ba5b
BA55: 20 f9 bb  JSR $bbf9
BA58: 4c 31 ba  JMP $ba31
BA5B: 29 7f     AND #$7f
BA5D: f0 0d     BEQ $ba6c
BA5F: a4 69     LDY $69
BA61: c0 05     CPY #$05
BA63: d0 cc     BNE $ba31
BA65: 4a        LSR A
BA66: f0 04     BEQ $ba6c
BA68: c6 68     DEC $68
BA6A: f0 c5     BEQ $ba31
BA6C: 18        CLC
BA6D: 69 06     ADC #$06
BA6F: 20 01 b9  JSR $b901
BA72: 4c 21 ba  JMP $ba21
BA75: a5 78     LDA $78
BA77: d0 14     BNE $ba8d
BA79: 06 71     ASL $71
BA7B: 2a        ROL A
BA7C: 06 71     ASL $71
BA7E: 2a        ROL A
BA7F: f0 09     BEQ $ba8a
BA81: 0a        ASL A
BA82: 0a        ASL A
BA83: 0a        ASL A
BA84: 0a        ASL A
BA85: 8d 06 d2  STA $d206
BA88: a9 84     LDA #$84
BA8A: 8d 07 d2  STA $d207
BA8D: 4c ff ba  JMP $baff
BA90: d6 6a     DEC $6a,X
BA92: d0 6b     BNE $baff
BA94: a1 5c     LDA ($5c,X)
BA96: f6 5c     INC $5c,X
BA98: d0 02     BNE $ba9c
BA9A: f6 5d     INC $5d,X
BA9C: a8        TAY
BA9D: f0 21     BEQ $bac0
BA9F: c9 0d     CMP #$0d
BAA1: d0 15     BNE $bab8
BAA3: b5 5c     LDA $5c,X
BAA5: 85 64     STA $64
BAA7: b5 5d     LDA $5d,X
BAA9: 85 65     STA $65
BAAB: a0 00     LDY #$00
BAAD: b1 64     LDA ($64),Y
BAAF: 95 5c     STA $5c,X
BAB1: c8        INY
BAB2: b1 64     LDA ($64),Y
BAB4: 95 5d     STA $5d,X
BAB6: d0 dc     BNE $ba94
BAB8: b0 06     BCS $bac0
BABA: 20 f9 bb  JSR $bbf9
BABD: 4c 94 ba  JMP $ba94
BAC0: 95 83     STA $83,X
BAC2: b4 72     LDY $72,X
BAC4: d0 1b     BNE $bae1
BAC6: a0 a3     LDY #$a3
BAC8: c9 00     CMP #$00
BACA: f0 10     BEQ $badc
BACC: 9d 00 d2  STA $d200,X
BACF: a5 69     LDA $69
BAD1: c9 06     CMP #$06
BAD3: d0 03     BNE $bad8
BAD5: 98        TYA
BAD6: d0 04     BNE $badc
BAD8: bd 4e b9  LDA $b94e,X
BADB: a8        TAY
BADC: 94 6b     STY $6b,X
BADE: 9d 01 d2  STA $d201,X
BAE1: a1 5c     LDA ($5c,X)
BAE3: d0 12     BNE $baf7
BAE5: a5 69     LDA $69
BAE7: d0 08     BNE $baf1
BAE9: a9 03     LDA #$03
BAEB: 20 01 b9  JSR $b901
BAEE: 4c 03 bb  JMP $bb03
BAF1: 20 16 bc  JSR $bc16
BAF4: 4c 03 bb  JMP $bb03
BAF7: 95 6a     STA $6a,X
BAF9: f6 5c     INC $5c,X
BAFB: d0 02     BNE $baff
BAFD: f6 5d     INC $5d,X
BAFF: ca        DEX
BB00: ca        DEX
BB01: 10 8d     BPL $ba90
BB03: a2 06     LDX #$06
BB05: b4 72     LDY $72,X
BB07: f0 0b     BEQ $bb14
BB09: b9 2c bb  LDA $bb2c,Y
BB0C: 48        PHA
BB0D: b9 22 bb  LDA $bb22,Y
BB10: 48        PHA
BB11: a5 55     LDA $55
BB13: 60        RTS
BB14: ca        DEX
BB15: ca        DEX
BB16: 10 ed     BPL $bb05
BB18: 68        PLA
BB19: a8        TAY
BB1A: 68        PLA
BB1B: aa        TAX
BB1C: a5 cc     LDA $cc
BB1E: 8d 0e d4  STA $d40e
BB21: 68        PLA
BB22: 40        RTI
BBF9: b4 73     LDY $73,X
BBFB: f0 0b     BEQ $bc08
BBFD: d6 73     DEC $73,X
BBFF: d0 09     BNE $bc0a
BC01: f6 5c     INC $5c,X
BC03: d0 10     BNE $bc15
BC05: f6 5d     INC $5d,X
BC07: 60        RTS
BC08: 95 73     STA $73,X
BC0A: a1 5c     LDA ($5c,X)
BC0C: 18        CLC
BC0D: 75 5c     ADC $5c,X
BC0F: 95 5c     STA $5c,X
BC11: b0 02     BCS $bc15
BC13: d6 5d     DEC $5d,X
BC15: 60        RTS
BC16: a9 00     LDA #$00
BC18: 85 66     STA $66
BC1A: a2 06     LDX #$06
BC1C: b4 72     LDY $72,X
BC1E: d0 03     BNE $bc23
BC20: 9d 01 d2  STA $d201,X
BC23: ca        DEX
BC24: ca        DEX
BC25: 10 f5     BPL $bc1c
BC27: 60        RTS
