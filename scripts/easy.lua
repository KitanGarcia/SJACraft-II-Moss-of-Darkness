-- Put your Lua here
function debugprint(s)
    if false then
        print(s)
    end --set false to disable text
end

---Determines if a function call returns true, and returns the value 1/0 accordingly. 
-- Used to count the number of successful commands pushed onto the commandqueue (used to limit # of commands the AI can push)
-- @param A - ActionAssigned
-- @param B - true or false.
function getCommandStatus(B)
    if B == true then
        debugprint("TRUE")
        return 1
    end
    debugprint("FALSE")
    return 0
end


--- Determines the behaviours that the AI will perform
-- ANY function that should modify command is expected to return true or false upon completion for proper execution
function CalculateCommand()
    brain = require("./scripts/brain")
    ActionAssigned = 0
    maxAssignment = 1
    if GetFoundAssetCount(AIPointer, "GoldMine") == 0 then
        debugprint("searchmap")
        SearchMap()
    elseif GetPlayerAssetCount(AIPointer, "TownHall") == 0 and GetPlayerAssetCount(AIPointer, "Keep") == 0 and GetPlayerAssetCount(AIPointer, "Castle") == 0 then
        debugprint("bth")
        BuildTownHall()
    elseif Safe() then
        debugprint("Safe")
        FarmCount = GetPlayerAssetCount(AIPointer, "Farm")
        peasantCount = GetPlayerAssetCount(AIPointer, "Peasant")
        BarracksCount = GetPlayerAssetCount(AIPointer, "Barracks")
        FootmanCount = GetPlayerAssetCount(AIPointer, "Footman")
        ArcherCount = GetPlayerAssetCount(AIPointer, "Archer") + GetPlayerAssetCount(AIPointer, "Ranger")
        consume = GetPlayerFoodConsumption(AIPointer)
        produce = GetPlayerFoodProduction(AIPointer)

        if GetSingleAssetWithCapability(AIPointer, "Mine") > -1 and ActionAssigned < maxAssignment then
            debugprint(peasantCount)
            ActionAssigned = ActionAssigned + getCommandStatus(ActivatePeasants())
        end

        if GetPlayerAssetCount(AIPointer, "Peasant") < 5 and ActionAssigned < maxAssignment then
            debugprint("ap")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
        end

        if ActionAssigned < maxAssignment and FarmCount == 0 and peasantCount > 2 and ActionAssigned < maxAssignment then
            debugprint("bbing")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end

        if ActionAssigned < maxAssignment and consume >= produce and ActionAssigned < maxAssignment then
            debugprint("bbing")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end

        if ActionAssigned < maxAssignment and ActionAssigned < maxAssignment and BarracksCount == 0 and FarmCount > 0 and peasantCount > 2 		then
            debugprint("bbarack")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Barracks"))
        end

        if ActionAssigned < maxAssignment and BarracksCount > 0 and FootmanCount < 6 then
            debugprint("Tfoot")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
        end

        SwitchToggle(AIPointer, 0)
        --print("toggle",  GetToggle(AIPointer, 0) )
        if ActionAssigned < maxAssignment and FootmanCount >= 6 and GetToggle(AIPointer, 0) then
            debugprint("AE")
            ActionAssigned = ActionAssigned + AttackEnemies(1)
        end
        
        if ActionAssigned < maxAssignment and peasantCount < 10 and peasantCount * 2 < produce then
            debugprint("ap")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
        end


    else
        debugprint("unsafe")
        FarmCount = GetPlayerAssetCount(AIPointer, "Farm")
        debugprint("farm")
        peasantCount = GetPlayerAssetCount(AIPointer, "Peasant")
        BarracksCount = GetPlayerAssetCount(AIPointer, "Barracks")
        FootmanCount = GetPlayerAssetCount(AIPointer, "Footman")
        ArcherCount = GetPlayerAssetCount(AIPointer, "Archer") + GetPlayerAssetCount(AIPointer, "Ranger")
        consume = GetPlayerFoodConsumption(AIPointer)
        produce = GetPlayerFoodProduction(AIPointer)
        
        if ActionAssigned < maxAssignment and consume >= produce then
            debugprint("bbing")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end

        if ActionAssigned < maxAssignment and BarracksCount == 0 and peasantCount > 0 then
            debugprint("bbarack")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Barracks"))
        end

        SwitchToggle(AIPointer, 0)
        if ActionAssigned < maxAssignment and GetToggle(AIPointer, 0) then
            debugprint("Attack")
            ActionAssigned = ActionAssigned + AttackEnemies(1)
        end

        if peasantCount == 0 and ActionAssigned < maxAssignment then
            debugprint("ap")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
        end

        if ActionAssigned < maxAssignment and BarracksCount > 1 then
            debugprint("TUfoot")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
            debugprint("TUfeet*")
        end
    end
end
