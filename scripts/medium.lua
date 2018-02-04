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
        return 1
    end
return 0
end


--- Determines the behaviours that the AI will perform
-- ANY function that should modify command is expected to return true or false upon completion for proper execution
function CalculateCommand()
    ActionAssigned = 0
    maxAssignment = 2
    brain = require("./scripts/brain")
    if GetFoundAssetCount(AIPointer, "GoldMine") == 0 then
        debugprint("searchmap")
        SearchMap()
    elseif GetPlayerAssetCount(AIPointer, "TownHall") == 0 and GetPlayerAssetCount(AIPointer, "Keep") == 0 and GetPlayerAssetCount(AIPointer, "Castle") == 0 then
        debugprint("Build Town Hall")
        BuildTownHall()
    elseif Safe() then
        debugprint("Safe") 
        FarmCount = GetPlayerAssetCount(AIPointer, "Farm")
        BarracksCount = GetPlayerAssetCount(AIPointer, "Barracks")
        LumberMillCount = GetPlayerAssetCount(AIPointer, "LumberMill")
        BlacksmithCount = GetPlayerAssetCount(AIPointer, "Blacksmith")
        ScoutTowerCount = GetPlayerAssetCount(AIPointer, "ScoutTower")
        peasantCount = GetPlayerAssetCount(AIPointer, "Peasant")
        FootmanCount = GetPlayerAssetCount(AIPointer, "Footman") +  GetPlayerAssetCount(AIPointer, "Knight")
        ArcherCount = GetPlayerAssetCount(AIPointer, "Archer") + GetPlayerAssetCount(AIPointer, "Ranger")
        
        consume = GetPlayerFoodConsumption(AIPointer)
        produce = GetPlayerFoodProduction(AIPointer) 

        SwitchToggle(AIPointer, 0)
        if GetSingleAssetWithCapability(AIPointer, "Mine") > -1 and ActionAssigned < maxAssignment then
            debugprint(peasantCount)
            ActionAssigned = ActionAssigned + getCommandStatus(ActivatePeasants())
        end
        -- Train Peasant
        if GetPlayerAssetCount(AIPointer, "Peasant") < 11 and ActionAssigned < maxAssignment then
            debugprint("Activate Peasant")
            debugprint(peasantCount)
            ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
        end
        -- Build Farm
        if ActionAssigned < maxAssignment and FarmCount == 0 and peasantCount >= 6 then
            debugprint("Build Farm")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end
        -- Build Farm (consume >= produce)
        if ActionAssigned < maxAssignment and consume+2 >= produce then
            debugprint("Build Farm: Consume >= Produce")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end
        -- Build Barracks
        if ActionAssigned < maxAssignment and ActionAssigned < maxAssignment and BarracksCount < 2 and FarmCount > 0 and peasantCount >= 6 then
            debugprint("Build Barracks" .. BarracksCount)
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Barracks"))
        end
        -- Build Lumber Mill
        if ActionAssigned < maxAssignment and ActionAssigned < maxAssignment and LumberMillCount == 0 and BarracksCount >= 2 and peasantCount >= 6 and FootmanCount >= 3 then
            debugprint("Build Lumber Mill")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("LumberMill"))
        end
        -- Build Blacksmith
        if ActionAssigned < maxAssignment and ActionAssigned < maxAssignment and BlacksmithCount == 0 and LumberMillCount >= 1 then
            debugprint("Build Blacksmith")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Blacksmith"))
        end  
        -- Build Scout Tower
        if ActionAssigned < maxAssignment and ActionAssigned < maxAssignment and BlacksmithCount >= 1 and ScoutTowerCount < 1 then
            debugprint("Build Scout Tower")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("ScoutTower"))
        end              
        -- Train Footmen
        if ActionAssigned < maxAssignment and BarracksCount > 0 and FootmanCount <= 12 and peasantCount >= 6 then
            debugprint("Train Footmen" .. FootmanCount)
            ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
        end
        -- Train Archer
        if GetUpgradeStatus(AIPointer, "ArrowUpgrade2") == false and ActionAssigned < maxAssignment and ArcherCount <= 3 and peasantCount >= 6 and BarracksCount > 0 and BlacksmithCount >= 1 then
            debugprint("Train Archer")
            debugprint(ArcherCount)
            ActionAssigned = ActionAssigned + getCommandStatus(TrainArcher())
        end

		-- Attack with Army
        if ActionAssigned < maxAssignment and FootmanCount >= 12 and ArcherCount >= 3 and GetToggle(AIPointer, 0) then
            debugprint("Attack Enemies")
            ActionAssigned = ActionAssigned + AttackEnemies(maxAssignment-ActionAssigned)
        end


        -- Begin Upgrades
        if FarmCount >= 1 and BarracksCount >= 1 and LumberMillCount >= 1 and BlacksmithCount >= 1 and ScoutTowerCount >= 1 then 
            -- Town Hall Upgrade            

            if ActionAssigned < maxAssignment and GetPlayerAssetCount(AIPointer, "Keep") == 0 and peasantCount >= 6 and FootmanCount >= 5 and ArcherCount >= 3 then
                debugprint("Upgrade TownHall")
                ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildKeep"))
            end            
            
            -- Scout Tower Upgrade
            if ActionAssigned < maxAssignment and GetPlayerAssetCount(AIPointer, "Keep") > 0 and BlacksmithCount >= 1 and FootmanCount >= 5 and GetPlayerAssetCount(AIPointer, "GuardTower") < ScoutTowerCount then
                debugprint("Upgrade Scout Tower")
                ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildGuardTower"))    
            end
            -- Blacksmith Upgrade
            if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"WeaponUpgrade2") == false and GetPlayerAssetCount(AIPointer, "Keep") > 0 and GetPlayerAssetCount(AIPointer, "GuardTower") > 0 then
                debugprint("Upgrade Weapons")
                ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("WeaponUpgrade2"))
            end
            -- Archer Upgrade
            if ActionAssigned < maxAssignment and LumberMillCount >= 1 and GetUpgradeStatus(AIPointer,"WeaponUpgrade2") and GetUpgradeStatus(AIPointer,"ArrowUpgrade2") == false and ArcherCount >= 2 then
                debugprint("Upgrade Arrows")
                ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("ArrowUpgrade2"))
            end 

            if ActionAssigned < maxAssignment and not GetUpgradeStatus(AIPointer, "ArmorUpgrade2") and GetUpgradeStatus(AIPointer,"ArrowUpgrade2") and GetUpgradeStatus(AIPointer,"WeaponUpgrade2") or false then
				debugprint("Upgrade Armor")
				ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("ArmorUpgrade2"))
			end
        end
        
        --not much to do (nothing to attack and all research done)
        if peasantCount <10 and ActionAssigned < maxAssignment then
            debugprint("ap")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
        end

        if ActionAssigned < maxAssignment and BarracksCount > 1 then
            debugprint("TUfoot")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
            debugprint("TUfeet*")
        end
        
    else
        debugprint("unsafe")
        FarmCount = GetPlayerAssetCount(AIPointer, "Farm")
	    debugprint("farm")
        BarracksCount = GetPlayerAssetCount(AIPointer, "Barracks")
	    debugprint("barracks")
        LumberMillCount = GetPlayerAssetCount(AIPointer, "LumberMill")
	    debugprint("lumbermill")
        BlacksmithCount = GetPlayerAssetCount(AIPointer, "Blacksmith")
        ScoutTowerCount = GetPlayerAssetCount(AIPointer, "ScoutTower")

        peasantCount = GetPlayerAssetCount(AIPointer, "Peasant")
        FootmanCount = GetPlayerAssetCount(AIPointer, "Footman")
        ArcherCount = GetPlayerAssetCount(AIPointer, "Archer") + GetPlayerAssetCount(AIPointer, "Ranger")

        consume = GetPlayerFoodConsumption(AIPointer)
        produce = GetPlayerFoodProduction(AIPointer)

debugprint(ActionAssigned .. " " .. maxAssignment)
        if ActionAssigned <= maxAssignment then
	    
            ActionAssigned = ActionAssigned + AttackEnemies(maxAssignment-ActionAssigned)
        end

        if ActionAssigned <= maxAssignment and GetSingleAssetWithCapability(AIPointer, "Mine") > -1 then
            debugprint("Activate Peasants")
            ActionAssigned = ActionAssigned + getCommandStatus(ActivatePeasants())
        end

        if ActionAssigned < maxAssignment and consume >= produce then
            debugprint("Build Farm: Consume >= Produce")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end

        if ActionAssigned < maxAssignment and BarracksCount == 0 and peasantCount > 0 then
            debugprint("Build Baracks")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Barracks"))
        end

        if ActionAssigned < maxAssignment and BarracksCount > 0 and FootmanCount < 10 then
            debugprint("Train Footmen")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
        end

        if ActionAssigned < maxAssignment and BarracksCount > 0 and ArcherCount  < 3 and FootmanCount > 9 then
            debugprint("Train Archer")
            ActionAssigned = ActionAssigned + getCommandStatus(TrainArcher())
        end
    end
end
