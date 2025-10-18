; In Setup Wizard:
;   Step Calibration:
;       X:  6 steps/mm
;       Y:  5 steps/mm
;   Limit Switches: Enabled

G21 ; millimeters
G91 ; relative coordinates
G17 ; XY plane
G94 ; units per minute feed rate mode

; Pencil Down
M5 S1000

; Create rectangle: YX: 50*100
F10
G1 Y50
G1 X100

G1 Y-50
G1 X-100

; Pencil Up
M3