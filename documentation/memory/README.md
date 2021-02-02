# Memory manipulation
### Read a value from memory
- After openning the process handle, we can use it to read process memory:
```autoit
    $value = KeReadInt($hProcess, 0x384DCF50)
```
- The $value should be a number (int) value at ```0x384DCF50```
- You can use these functions to read other types of value: ```KeReadFloat(), KeReadDouble(), KeReadWord(), KeReadFloat()```
- Use ```KeReadString()``` and ```KeReadStringUnicode()``` to read a string:
```autoit
    $string = KeReadString($hProcess, 0x384DCF50, $stringLength=0)
```
- If the ```$stringLength``` is set to ```0```, it'll auto detect the length
- If you want to read a different type of value, you can use:
```autoit
    $value = KeRead($hProcess, 0x384DCF50, $type="int")
```
- See availables types of value [here](https://www.autoitscript.com/autoit3/docs/functions/DllStructCreate.htm)

<br>

### Write a value to memory
- To write ```999``` to the address ```0x384DCF50```, use:
```autoit
    KeWriteInt($hProcess, 0x384DCF50, 999)
```
- You can use these functions to write other types of value: ```KeWriteFloat(), KeWriteDouble(), KeWriteWord(), KeWriteFloat()```
- Use ```KeWriteString()``` and ```KeWriteStringUnicode()``` to write a string:
```autoit
    KeWriteString($hProcess, 0x384DCF50, "hello world", $stringLength=0)
```
- If the ```$stringLength``` is set to ```0```, it'll write the string with a ```NULL``` byte at the end, if it is set to ```-1```, it'll write the string without the ```NULL``` byte.
- If you want to read a different type of value, you can use:
```autoit
    KeWrite($hProcess, 0x384DCF50, 999, $type="int")
```
- See availables types of value [here](https://www.autoitscript.com/autoit3/docs/functions/DllStructCreate.htm)
