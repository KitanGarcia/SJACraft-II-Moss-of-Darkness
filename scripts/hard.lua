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
    maxAssignment = 4
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
        ScoutTowerCount = GetPlayerAssetCount(AIPointer, "ScoutTower") + GetPlayerAssetCount(AIPointer, "GuardTower") + GetPlayerAssetCount(AIPointer, "CannonTower")  
	GuardTowerCount = GetPlayerAssetCount(AIPointer, "GuardTower")
		
        peasantCount = GetPlayerAssetCount(AIPointer, "Peasant")
        FootmanCount = GetPlayerAssetCount(AIPointer, "Footman")+ GetPlayerAssetCount(AIPointer, "Knight")
        ArcherCount = GetPlayerAssetCount(AIPointer, "Archer") + GetPlayerAssetCount(AIPointer, "Ranger")
	NumFighters = FootmanCount + ArcherCount
        
        consume = GetPlayerFoodConsumption(AIPointer)
        produce = GetPlayerFoodProduction(AIPointer) 
	
	--Activate Peasants
	SwitchToggle(AIPointer, 0)
	if ActionAssigned < maxAssignment and GetToggle(AIPointer, 0) then
            debugprint(peasantCount)
            ActionAssigned = ActionAssigned + getCommandStatus(ActivatePeasants())
        end
	--Have footman patrol
	--if ActionAssigned < maxAssignment and NumFighters > 0 and NumFighters < 15 then
	 -- debugprint("Patrol")
	  --Bryan's patrol function
	--end
	
	SwitchToggle(AIPointer, 1)
	if ActionAssigned < maxAssignment and NumFighters > 8 and GetToggle(AIPointer, 1) then
	  ActionAssigned = ActionAssigned + AttackEnemies(maxAssignment-ActionAssigned)
	end
	--Train Peasant
	if ActionAssigned < maxAssignment and peasantCount < 5 then
	   debugprint("TrainPeasant")
	   ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
           --if peasantCount < 3 then
	     -- ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
	   --elseif peasantCount < 5 and BarracksCount > 0 then 
             -- ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
	   --elseif peasantCount < 7 and LumberMillCount > 0 then
	     -- ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
	   --elseif GetPlayerAssetCount(AIPointer, "Keep") > 0 then
	     -- ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
	   --end
	end
	-- Build Farm
        if ActionAssigned < maxAssignment and FarmCount == 0 and peasantCount >= 6 then
            debugprint("Build Farm")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end
        -- Build Farm (consume >= produce)
        if ActionAssigned < maxAssignment and consume+3 >= produce then
            debugprint("Build Farm: Consume >= Produce")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Farm"))
        end
	--Build Barrack
	if ActionAssigned < maxAssignment and BarracksCount < 1 then
	   debugprint("Build Barracks")
	   debugprint(BarracksCount)
           ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Barracks"))
	end
	if ActionAssigned < maxAssignment and BarracksCount < 2 and BlacksmithCount == 1 then
	   debugprint("Build Barracks")
	   debugprint(BarracksCount)
           ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Barracks"))
	end
	if ActionAssigned < maxAssignment and BarracksCount < 3 and ScoutTowerCount > 2 then
	   debugprint("Build Barracks")
	   debugprint(BarracksCount)
           ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Barracks"))
	end
	
	--Build LumberMill
	if ActionAssigned < maxAssignment and LumberMillCount == 0 then
	   debugprint("Build Lumber Mill")
           ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("LumberMill"))
	end
	--Build BlackSmith
	if ActionAssigned < maxAssignment and BlacksmithCount == 0 and LumberMillCount == 1 then
	   debugprint("Build BlackSmith")
           ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("Blacksmith"))
	end
	--Build ScoutTower
	if ActionAssigned < maxAssignment and LumberMillCount >= 1 and ScoutTowerCount < 6 then
	   debugprint("Build Scout Tower")
            ActionAssigned = ActionAssigned + getCommandStatus(BuildBuildings("ScoutTower"))
	end
	--Train Footmen
	if ActionAssigned < maxAssignment and FootmanCount < 11 and BarracksCount > 0 then
	   debugprint("TrainFootmen")
	   --for i = 1, BarracksCount,3 do
		--debugprint("GettingStuck?")
		if ActionAssigned < maxAssignment and FootmanCount < 10 and BarracksCount > 0 then
		  ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
		end
	   --end
	end
	--Train Archer/Ranger
	if ActionAssigned < maxAssignment and (ArcherCount) < 10 and LumberMillCount > 0 and BarracksCount > 0 then 
	   if ActionAssigned < maxAssignment and (ArcherCount) < 5 and LumberMillCount > 0 then
	      ActionAssigned = ActionAssigned + getCommandStatus(TrainArcher())
	   elseif ActionAssigned < maxAssignment and ArcherCount < 12 and ScoutTowerCount > 0 and LumberMillCount > 0 then
	      ActionAssigned = ActionAssigned + getCommandStatus(TrainArcher())
	   end
	end
	--check that upgrades can be done
	if ActionAssigned < maxAssignment and FarmCount >= 1 and BarracksCount >= 1 and LumberMillCount >= 1 and BlacksmithCount >= 1 and ScoutTowerCount >= 0 then 
		--Upgrade to Keep
		if ActionAssigned < maxAssignment and GetPlayerAssetCount(AIPointer,"Keep") == 0 then
		   debugprint("Upgrade TownHall")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildKeep"))
		end
	
		--Upgrade Arrow2
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"ArrowUpgrade2") == false and LumberMillCount > 0 and GetPlayerAssetCount(AIPointer, "Keep") > 0 then
		   debugprint("Upgrade Arrows2")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("ArrowUpgrade2"))
		end
		--Build GuardTower
		if ActionAssigned < maxAssignment and GuardTowerCount < 3 and ScoutTowerCount > 0 and GetUpgradeStatus(AIPointer, "BuildGuardTower") == false then
		   debugprint("Build GuardTower")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildGuardTower"))
		end
		--Upgrade Armor2
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"ArmorUpgrade2") == false and GetUpgradeStatus(AIPointer,"ArrowUpgrade2") and BlacksmithCount > 0 then
		   debugprint("Upgrade Armor2")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("ArmorUpgrade2"))
		end
		--Ranger Research
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"BuildRanger") == false and GetUpgradeStatus(AIPointer,"ArmorUpgrade2") and LumberMillCount > 0 then
		   debugprint("Research Ranger")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildRanger"))
		end
		--Upgrade Arrow3
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"ArrowUpgrade3") == false and GetUpgradeStatus(AIPointer,"BuildRanger") then
		   debugprint("Upgrade Arrows3")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("ArrowUpgrade3"))
		end
		--Armor Upgrade3
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"ArmorUpgrade3") == false and GetUpgradeStatus(AIPointer,"BuildRanger") and BlacksmithCount > 0 then
		   debugprint("Upgrade Armor3")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("ArmorUpgrade3"))
		end
		-- Longbow
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"Longbow") == false and LumberMillCount > 0 then
		   debugprint("Research Longbow")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("Longbow"))
		end
		-- Ranger Scouting
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"RangerScouting") == false and GetUpgradeStatus(AIPointer,"Longbow") and LumberMillCount > 0 then
		   debugprint("Ranger Scouting")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("RangerScouting"))
		end
	 	-- Marksmanship
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"Marksmanship") == false and  GetUpgradeStatus(AIPointer,"RangerScouting") and LumberMillCount > 0 then
		   debugprint("Marksmanship")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("Marksmanship"))
		end
		--Build Knight
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"BuildKnight") == false and BlacksmithCount > 0 then 
		   debugprint("research knight")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildKnight"))
		end
		--Weapon Upgrade2
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"WeaponUpgrade2") == false and GetUpgradeStatus(AIPointer,"BuildKnight") and BlacksmithCount > 0 then
		   debugprint("Upgrade Weapon2")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("WeaponUpgrade2"))
		end
		--Build Cannon Tower
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer, "BuildCannonTower") == false  and ScoutTowerCount > 0 and  GetUpgradeStatus(AIPointer, "WeaponUpgrade2") then
		   debugprint("Build Cannon Tower")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildCannonTower"))
		end
		--Build Castle
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"BuildCastle") == false and GetUpgradeStatus(AIPointer,"WeaponUpgrade2") then
		   debugprint("Build Castle")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("BuildCastle"))
		end
		--Upgrade Weapon3
		if ActionAssigned < maxAssignment and GetUpgradeStatus(AIPointer,"WeaponUpgrade3") == false and GetUpgradeStatus(AIPointer,"WeaponUpgrade2") and BlacksmithCount > 0 then
		   debugprint("Upgrade Weapon3")
		   ActionAssigned = ActionAssigned + getCommandStatus(BuildUpgrade("WeaponUpgrade3"))
		end
	end
	
	--not much to do (nothing to attack and all research done)
		if peasantCount < 10 and peasantCount*2 < produce and ActionAssigned < maxAssignment then
			debugprint("ap")
			ActionAssigned = ActionAssigned + getCommandStatus(TrainPeasant())
		end

		if ActionAssigned < maxAssignment and BarracksCount > 1 and GetToggle(AIPointer, 2) then
			debugprint("TUfoot")
			SwitchToggle(AIPointer, 2)			
			ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
			debugprint("TUfeet*")
		end

		if ActionAssigned < maxAssignment and BarracksCount > 1 and not GetToggle(AIPointer, 2) then
			debugprint("TUfoot")
			SwitchToggle(AIPointer, 2)			
			ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
			debugprint("TUfeet*")
		end
	
    --Unsafe	  
    else
	debugprint("unsafe")
	FarmCount = GetPlayerAssetCount(AIPointer, "Farm")
	debugprint("farm")
        BarracksCount = GetPlayerAssetCount(AIPointer, "Barracks")
	debugprint("barracks")
        LumberMillCount = GetPlayerAssetCount(AIPointer, "LumberMill")
	debugprint("lumbermill")
        BlacksmithCount = GetPlayerAssetCount(AIPointer, "Blacksmith")
        ScoutTowerCount = GetPlayerAssetCount(AIPointer, "ScoutTower") + GetPlayerAssetCount(AIPointer, "GuardTower") + GetPlayerAssetCount(AIPointer, "CannonTower")  
		GuardTowerCount = GetPlayerAssetCount(AIPointer, "GuardTower")
        peasantCount = GetPlayerAssetCount(AIPointer, "Peasant")
        FootmanCount = GetPlayerAssetCount(AIPointer, "Footman")
        ArcherCount = GetPlayerAssetCount(AIPointer, "Archer") + GetPlayerAssetCount(AIPointer, "Ranger")

        consume = GetPlayerFoodConsumption(AIPointer)
		produce = GetPlayerFoodProduction(AIPointer)

		SwitchToggle(AIPointer, 1)		
		if ActionAssigned < maxAssignment and GetToggle(AIPointer, 1) then
			ActionAssigned = ActionAssigned + AttackEnemies(maxAssignment-ActionAssigned)
		end
		if ActionAssigned < maxAssignment and GetSingleAssetWithCapability(AIPointer, "Mine") > -1 then
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

		if ActionAssigned < maxAssignment and BarracksCount > 0 then
			debugprint("Train Footmen")
			ActionAssigned = ActionAssigned + getCommandStatus(TrainFootmen())
		end

		if ActionAssigned < maxAssignment and BarracksCount > 0 and ArcherCount  < 3 and FootmanCount > 9 then
			debugprint("Train Archers or Rangers")
		ActionAssigned = ActionAssigned + getCommandStatus(TrainArcher())
		end
    end
end
