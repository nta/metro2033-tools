:: vs2008
del *.ncb /S
del *.suo /S /A:H
del *.user /S
del *.pdb /S
:: vs2010
del *.sdf /S
::
rd binaries /S /Q
rd intermediate /S /Q
pause