## ED4scan Version History
|version  | comment|
|-------- | --------|
|v0.5.5   | Added new SOC readout from EVC and information about battery cooling / heating systems.|
|         | + small bug fixes.|
|v0.5.2   | Minimized menu output to save memory and moved some constants for more project flexibility.|
|         | INT pin for MCP CAN controller now defined by a constant.|
|         | + some small bug fixes.|
|v0.5.1   | **Please also reload library files**|
|         | Added support for the Smart version of the Renault "Chameleon Charger".|
|         | You have to compile the tool for the 22kW version: in _OBL_dfs.h set the constant FASTCHG = true (see installation in Wiki).|
|v0.4.4   | **Please also reload library files**|
|         | THX to MineCooky I was able to adapt to the new BMS rev. for the EQ series. 22kW charger will be implemented next.|
|         | In addition minor bug fixes are included: AvgNew-Library optimized, warnings fixed.|
|v0.4.3   | THX to input from Jim, I fixed all warnings in ED4scan and optimized 7kW OBC test. Reviewing Libs in some days...|
|v0.4.2   | 1st public release|
