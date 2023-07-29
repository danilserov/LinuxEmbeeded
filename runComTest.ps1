$port = new-Object System.IO.Ports.SerialPort COM5, 8000, None, 8, one
$port.open()
$port.WriteLine("string0")
$port.WriteLine("string1")
$port.WriteLine("string2")
$port.WriteLine("string3")
$port.Write(" first string4 to wait")
Write-Output "wait 1"
Start-Sleep -Seconds 1
$port.Write(" second string4 to wait")
Write-Output "wait 5"
Start-Sleep -Milliseconds 5001
Write-Output "end wait!"
$port.WriteLine("string5")
$port.WriteLine("string6")
$port.WriteLine("string7")
$port.WriteLine("string8")
$port.WriteLine("string9")

$port.WriteLine("string10")
$port.WriteLine("string11")
$port.WriteLine("string12")
$port.close()