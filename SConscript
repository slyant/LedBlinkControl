from building import *
Import('rtconfig')

src = []
path = []
cwd = GetCurrentDir()

if GetDepend('PKG_USING_LED_BLINK_CONTROL'):
    src += Glob('*.c')
    path += [cwd]

# add src and include to group.
group = DefineGroup('LedBlinkControl', src, depend = ['PKG_USING_LED_BLINK_CONTROL'], CPPPATH = path)

Return('group')
