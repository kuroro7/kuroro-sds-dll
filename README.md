## kuroro-sds-dll

This is a first step for who those want to make DLL injection possible in `seiya.exe` file.

It comes with two basic features:

* The window border removed to work well with lyre version
* Return from `GetHostMoneyInPack` lua function improved so we can get `stars` and `gems` well

To get the current player stars amount in `.lua` files, you just need to call the function:

```lua
money_info = GameApi.GetHostMoneyInPack()

money_info[1] -- silver
money_info[2] -- gold
money_info[3] -- stars
money_info[4] -- gems
```

Feel free to add or change in anyway you want, and if you want to show me your progress or ask me anything, just email me.

**THIS MUST BE COMPILED TO x86 ARCHITECTURE SINCE IS THE SAME ARCHITECTURE OF THE EXE FILE**

To use this DLL, you must use kuroro-dll-patcher found [here](https://github.com/kuroro7/kuroro-sds-dll-injector)
