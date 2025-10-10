; In Setup Wizard:
;   Step Calibration: 10 steps/mm
;   Limit Switches: Enabled

G21 ; millimeters
G91 ; relative coordinates
G17 ; XY plane
G94 ; units per minute feed rate mode

; Pencil Up
M3 S1000

; Create rectangle: YX: 20*50
G1 Y20 F25
G1 X50

G1 Y-20
G1 X-50

; Pencil Down
M5