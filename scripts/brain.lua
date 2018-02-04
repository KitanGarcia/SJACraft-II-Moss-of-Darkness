function debugprint(s)
    if false then --set false to disable text
        print(s)
    end
end


--============================ Behaviours ============================================ IL: dont do --...- messes with luadoc

--- Selects first idle asset with movement capabilities and gives it a command
-- to move towards the nearest unseen tile
-- @return true if any unit is able to successfully complete this command
function Safe()
    -- debugprint("S")
    locationTable = GetAllPlayerBuildingAssets(AIPointer)
    -- debugprint("A")
    enemyTable = {}
    enemyTable = ConcatTables(enemyTable, GetLocationsOfAllEnemyTypes(AIPointer, "Footman"))
    enemyTable = ConcatTables(enemyTable, GetLocationsOfAllEnemyTypes(AIPointer, "Archer"))
    enemyTable = ConcatTables(enemyTable, GetLocationsOfAllEnemyTypes(AIPointer, "Ranger"))
    enemyTable = ConcatTables(enemyTable, GetLocationsOfAllEnemyTypes(AIPointer, "Knight"))
    enemyTable = ConcatTables(enemyTable, GetLocationsOfAllEnemyTypes(AIPointer, "Peasant"))

    -- debugprint(#locationTable .. " " .. #enemyTable)


    if #locationTable > 1 and #enemyTable > 1 then
        -- debugprint("1")

        for k = 1, #locationTable, 3 do
            -- debugprint("2")
            locationX = locationTable[k + 1]
            -- debugprint("3")
            locationY = locationTable[k + 2]
            minX = locationX - 5
            minY = locationY - 5
            maxX = locationX + 5
            maxY = locationY + 5

            for m = 1, #enemyTable, 3 do
                enemyX = enemyTable[m + 1]
                enemyY = enemyTable[m + 2]
                if enemyX >= minX and enemyX <= maxX and enemyY >= minY and enemyY <= maxY then return false end
            end
        end
    end

    return true
end

function Shelter()
    peasants = GetAllPlayerAssetsWithCapability(AIPointer, "BuildBarracks", "Mine" )
    TH = GetNearestAssetToOfType(AIPointer, peasants[1], "TownHall")
    if TH~=-1 then
    x, y = GetAssetTilePosition(TH)
    end
    if #peasants == 0 then 
	-- debugprint("Exit Shelter False")
	return false
    end
    SetCommandAction(AIPointer, CmdPointer, "Move")
    SetCommandTargetPos(AIPointer, CmdPointer, x, y)
    for i = 1, #peasants, 3 do
       AddCommandActor(AIPointer, CmdPointer, peasants[i])
    end 
    PushBackCommand(AIPointer, CmdPointer)
    -- debugprint("Exit Shelter True")
    return true
end
function min(a,b)
    if a > b then
        return b
    else
        return a
    end
end
--======================= Behaviours ============================================== 

--- Selects first idle asset with movement capabilities and gives it a command
-- to move towards the nearest unseen tile
-- @return true if any unit is able to successfully complete this command
function SearchMap ()
    --print("SearchMap")
    debugprint("SearchMap")

    movableAsset = GetSingleAssetWithCapability(AIPointer, "Move")
    if movableAsset ~= -1 then
        if GetNumberOfGoldMines(AIPointer) == GetFoundAssetCount(AIPointer, "GoldMine") then
            locationTable = GetLocationsOfAllAssetsOfType(AIPointer, "GoldMine")
            debugprint("numGoldMines" .. #locationTable/3)
            for i = 3, #locationTable/3 + 2, 1 do
                if not GetToggle(AIPointer, i)  then
                    x= locationTable[(i - 3) * 3 + 2]
                    y= locationTable[(i - 3) * 3 + 3]
                    SwitchToggle(AIPointer, i)
                    break
                end
            end
            --print("tyhbyh ")
            numPointer = min(#locationTable/3 + 2, 15)
            --print("mincall " .. numPointer)
            
            if GetToggle(AIPointer, numPointer) then
                for i =3, numPointer, 1 do
                    SwitchToggle(AIPointer, i)
                end
            end
        else
            x, y = GetNearestTileOfType(AIPointer, movableAsset, "None")        
        end
        --print("  Moving to " .. x .. ", " .. y)
        if(0 <= x) then
            GiveActorAction(movableAsset, "Move", x, y)
            PushBackCommand(AIPointer, CmdPointer)

            return true
        end
    end
    return false
end

--- Selects first idle asset with movement capabilities and gives it a command
-- to move towards the nearest unseen tile
-- @return true if any unit is able to successfully complete this command
function SearchForest ()
    --print("SearchMap")
    debugprint("SearchForest")
    TownHallNumber = 1 --  search around the ith Townhall
    THID, THX,THY = getTownhallPosition(TownHallNumber);
    movableAsset = GetSingleAssetWithCapability(AIPointer, "Move")
    if movableAsset ~= -1 then
        x, y = GetNearestTileOfType(AIPointer, THID,  "None")
        --print("  Moving to " .. x .. ", " .. y)
        if(0 <= x) then
            GiveActorAction(movableAsset, "Move", x, y)
            return true
        end
    end
    return false
end


function encodeConstraints(tuple)
    result = 0;
    if(tuple[1]==1) then
        result=result+1
    end
    if(tuple[2]==1) then
        result=result+2
    end
    if(tuple[3]==1) then
        result=result+4
    end
    return result
end
--- Find the best placement of a given building with Constraint Parameters
-- to search in a specific area whehter there is proper position for placement
-- @param builderAsset Id of the builder to build the building
-- @param HConstrants Horizontal search Constrants, {target'sleft,target'smiddle,target's right} eg. {0,1,0}
-- @param VConstrants vertical search Constrants, {target's above,target'smiddle,target's below}
-- @param targetX target's x
-- @param targetY target's y
-- @param offset Minimal offset between this building and the target
-- @param padding extra padding added to the size of the building to be built (min is 1)
-- @param assetType Type of the asset to be build
-- @param farthest the fartherst distance to search within
-- @return buildable,x,y bool is true if found pos x y; otherwise return false, and invalid x, y

function findBestPlacementWithConstraints(AIPointer,builderAsset,HConstrants,VConstrants,targetX,targetY,offset,padding,assetType,farthest)

    H = encodeConstraints(HConstrants)
    V = encodeConstraints(VConstrants)
    if padding < 1 then padding = 1 end
    posx,posy = FindAssetPlacementWithConstraints(AIPointer, builderAsset,H,V,targetX,targetY,offset,padding,assetType,farthest)

    return posx,posy
end


--- Helper function for findBestPlacementWithConstraints for townhall
-- @return buildable,x,y bool is true if found pos x y; otherwise return false, and invalid x, y

function findBestPlacementForTownHall(AIPointer, goldMineX, goldMineY,builder)
    --as close to gold mine as possible, no farther than 10 away, buildingtype townhall
    posx,posy = findBestPlacementWithConstraints(AIPointer,builder,{1,1,0},{1,1,1},goldMineX,goldMineY,0,1,"TownHall",10)

    return posx,posy
end

--- Helper function for findBestPlacementWithConstraints for Lumbermill
-- @return buildable,x,y bool is true if found pos x y; otherwise return false, and invalid x, y

function findBestPlacementForLumberMill(AIPointer, woodX, woodY,builder)
    --as close to forest as possible, no farther than 10 away, buildingtype lumbermill
    posx,posy = findBestPlacementWithConstraints(AIPointer,builder,{1,1,1},{1,1,1},woodX,woodY,0,1,"LumberMill",10)

    return posx,posy
end



--- Helper function to handle building of offensive barrack
-- search for enemy, and if spoted build a barrack safe distance away from enermy
-- @return true if any unit is able to scucessfully complete this command, or if searchmap returns true
function OffensiveBarrack()
    debugprint("Offensive Barrack")
    SafeDistanceFromEnermy = 15;
    --change this setting to make the this offensive building closer or further from enermy

--    BuilderAsset = GetSingleAssetWithCapability(AIPointer, "BuildBarracks")
    BuilderAsset = GetBuilder(AIPointer,"BuildBarracks", "Barracks")
    if BuilderAsset ~= -1 then
        buildAction = "BuildBarracks"

        TownhallNumber =1
        THID, TH_x,TH_y = getTownhallPosition(TownhallNumber)

        targetAsset = GetNearestEnemy(AIPointer, TH_x,TH_y,-1);
        if targetAsset == -1 then
            return SearchMap()
        else

            E_X, E_Y = GetNearestEnemyLocation(AIPointer,TH_x,TH_y,-1);

            PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,BuilderAsset,{1,1,1},{1,1,1},E_X,E_Y,SafeDistanceFromEnermy,1,"Barracks",18)

            CanBuild = CanInitiate(AIPointer, BuilderAsset, buildAction)

            if(0 > PlacementX) then

                return SearchMap()
            end
            if(CanBuild) then
                --print("builfding bsrrscks")
                --debugprint("Giving Action")
                GiveActorAction(BuilderAsset, "BuildBarracks", PlacementX, PlacementY)
                PushBackCommand(AIPointer, CmdPointer)
                return true
            end
              --print("cant build")
            return false
        end
    end
end

--- Helper function to handle building of offensive Tower
-- search for enemy, and if spoted build a Tower safe distance away from enermy
-- @return true if any unit is able to scucessfully complete this command, or if searchmap returns true
function OffensiveTower()
    debugprint("Offensive Tower ")

    SafeDistanceFromEnermy = 15;
    --change this setting to make the this offensive building closer or further from enermy

--    BuilderAsset = GetSingleAssetWithCapability(AIPointer, "BuildScoutTower")
    BuilderAsset = GetBuilder(AIPointer,"BuildScoutTower","ScoutTower")
    if BuilderAsset ~= -1 then
        buildAction = "BuildScoutTower"
        TownhallNumber =1
        THID, TH_x,TH_y = getTownhallPosition(TownhallNumber)
        targetAsset = GetNearestEnemy(AIPointer, TH_x,TH_y,-1);

        if targetAsset == -1 then
            return SearchMap()
        else

            E_X, E_Y = GetNearestEnemyLocation(AIPointer,TH_x,TH_y,-1);
            PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,BuilderAsset,{1,1,1},{1,1,1},E_X,E_Y,SafeDistanceFromEnermy,1,"ScoutTower",20)
            CanBuild = CanInitiate(AIPointer, BuilderAsset, buildAction)

            if(0 > PlacementX) then
                return SearchMap()
            end
            if(CanBuild) then
                GiveActorAction(BuilderAsset, "BuildScoutTower", PlacementX, PlacementY)
                PushBackCommand(AIPointer, CmdPointer)
                return true
            end
            return false
        end
    end
end



--- Selects first idle asset with build Lumber Mill capabilities and gives it a command
-- to build a town hall close to the nearest wood
-- @return true if any unit is able to scucessfully complete this command, or if searchmap returns true
function BuildLumberMill ()
    debugprint("BuildLumberMill")
    --print("BuildTownHall")
--    builderAsset = GetSingleAssetWithCapability(AIPointer, "BuildLumberMill")
    BuilderAsset = GetBuilder(AIPointer,"BuildLumberMill","LumberMill")
    -- print(" Builder Asset ID: " .. BuilderAsset)
    --debugprint("gotbuilder")
    if BuilderAsset ~= -1 then
        woodX, woodY = GetNearestTileOfType(AIPointer, BuilderAsset, "Forest")
        --debugprint("got forest pos")

        if (woodX ~= -1)and(woodY~=-1) then
            buildX, buildY = findBestPlacementForLumberMill(AIPointer,woodX, woodY, BuilderAsset)
            if (buildX ~= -1)and(buildY~=-1) then
                --assetX, assetY = GetAssetTilePosition(BuilderAsset)
                --debugprint("got thing")


                -- print("ForestPos:",woodX,woodY,"lumberMill pos",buildX,buildY)
                --debugprint("gotbuildpos")

                GiveActorAction(BuilderAsset, "BuildLumberMill", buildX, buildY)
                PushBackCommand(AIPointer, CmdPointer)
                --debugprint("hereyougo")
                return true
            end
        end
        -- print("Searchfor wood")
        return SearchForest()
    end
    return false
end




--- Selects first idle asset with build town hall capabilities and gives it a command
-- to build a town hall close to the nearest gold mine
-- @return true if any unit is able to scucessfully complete this command, or if searchmap returns true
function BuildTownHall ()
    debugprint("BuildTownHall")
    --print("BuildTownHall")
--    BuilderAsset = GetSingleAssetWithCapability(AIPointer, "BuildTownHall")
    BuilderAsset = GetBuilder(AIPointer,"BuildTownHall","TownHall")

    -- print(" Builder Asset ID: " .. BuilderAsset)
	debugprint("gotbuilder")
    if BuilderAsset ~= -1 then
        goldMine = GetNearestAssetToOfType(AIPointer, BuilderAsset, "GoldMine")
	    --debugprint("got goldmine")
	if goldMine then
        goldMineX, goldMineY = GetAssetTilePosition(goldMine)
	    end
    

	    --debugprint("got gm pos")        
	    --buildX, buildY = GetBestAssetPlacement(AIPointer, goldMineX, goldMineY, BuilderAsset, "TownHall", 1)

        buildX, buildY = findBestPlacementForTownHall(AIPointer,goldMineX, goldMineY, BuilderAsset)

        -- print("GoldMinePos:",goldMineX,goldMineY,"townHallpos",buildX,buildY)
        debugprint("gotbuildpos")
        if (buildX ~= -1)and(buildY~=-1) then
            --assetX, assetY = GetAssetTilePosition(BuilderAsset)
            --debugprint("got thing")
            GiveActorAction(BuilderAsset, "BuildTownHall", buildX, buildY)
            PushBackCommand(AIPointer, CmdPointer)
            debugprint("hereyougo")
            return true
        else
            return SearchMap()
        end
    end
    return false
end

--- Basic peasant functionality. Will tell peasants to gather mine by default and the harvest lumber
-- if there is enough gold, or there are more the enough miners
-- @param trainMore whether or not the town hall should train more peasants
-- @return true if any unit is able to complete this command or if a townhall trains a peasants.
function ActivatePeasants ()

    function FindForest(asset) return GetNearestTileOfType(AIPointer, asset, "Forest") end
    function FindGoldMine(asset) return GetNearestAssetToOfType(AIPointer, asset, "GoldMine") end


    debugprint("ActivatePeasants")
    goldMiners = CountAssetsWithAction(AIPointer, "MineGold") + CountAssetsWithAction(AIPointer, "ConveyGold")
    lumberHarvesters = CountAssetsWithAction(AIPointer, "HarvestLumber") + CountAssetsWithAction(AIPointer, "ConveyLumber")
    PANTS = GetLocationsOfIdleAssetsOfType(AIPointer, "Peasant")
    townHallAsset = GetSingleAssetWithCapability(AIPointer, "BuildPeasant")
    

    miningAsset = GetInterruptableAssetWithCapability(AIPointer, "MineGold", "Mine")
    lumberingAsset = GetInterruptableAssetWithCapability(AIPointer, "HarvestLumber", "Mine")

    idleAsset = -1
    if #PANTS > 0 then
       idleAsset = PANTS[1] 
    end
    
    switchToGold = false
    switchToLumber = false
    if goldMiners >= 2 and lumberHarvesters == 0 then
        switchToLumber = true
    elseif lumberHarvesters >= 2 and goldMiners == 0 then
    switchToGold = true
    end                    
    debugprint(idleAsset .. " " .. miningAsset .. " " .. lumberingAsset)
    if idleAsset ~= -1 or (miningAsset ~= -1 and switchToLumber) or (lumberingAsset ~= -1 and switchToLumber) then
        if idleAsset~= -1 and (GetAssetLumber(idleAsset) ~= 0 or GetAssetGold(idleAsset) ~= 0) and townHallAsset ~=-1 then
            debugprint("Convey")
	    
            townHallX, townHallY = GetAssetTilePosition(townHallAsset)
            for i = 1, #PANTS, 3 do
                idleAsset = PANTS[i]
                if( idleAsset~= -1 and (GetAssetLumber(idleAsset) ~= 0 or GetAssetGold(idleAsset) ~= 0) ) then
                    ConveyResources(idleAsset, townHallAsset, townHallX, townHallY)
	       	    end
        	end
            PushBackCommand(AIPointer, CmdPointer)
            return true
    	end
        if goldMiners ~= 0 and (GetPlayerGold(AIPointer) > GetPlayerLumber(AIPointer) * 2 or switchToLumber) then
            for i = 1, #PANTS, 3 do
               idleAsset=PANTS[i]
                   x,y = FindForest(idleAsset)
               if x >= 0 then 
                GiveActorAction(idleAsset, "Mine", x, y)
                PushBackCommand(AIPointer, CmdPointer)
                return true 
               end --looks for idle a peasant which can reach a forest
            end
            if miningAsset~=-1 and switchToLumber then
                x,y = FindForest(miningAsset)
               if x >= 0 and switchToLumber then 
                GiveActorAction(miningAsset, "Mine", x, y)
                PushBackCommand(AIPointer, CmdPointer)
               return true
               end
            end 
        else
            hasActor = false
            for i = 1, #PANTS, 3 do
               idleAsset=PANTS[i]
                    goldMine = FindGoldMine(idleAsset)
               if goldMine ~= -1 then 
					x, y = GetAssetTilePosition(goldMine)
               end
               if x >= 0 then 
               		hasActor = true
               		MineGold(idleAsset, x,y)
               end --looks for a idle peasant which can reach a goldmine
            end
            if lumberingAsset~=-1 and switchToGold then		    
                   goldMine = FindGoldMine(lumberingAsset)
               if goldMine ~= -1 then 
                            x, y = GetAssetTilePosition(goldMine)
               end
               if x >= 0 and switchToGold then
                hasActor = true
                MineGold(lumberingAsset, x,y)
               end
            end          	
            -- print("has actor? ", hasActor)		
            if hasActor then 
                PushBackCommand(AIPointer, CmdPointer)
                return true
            end 
               

		end
    end
    return false
end

--- Command unused asset which can build the desired upgrade to do so. If the upgrade is already being researched, returns false.
--  If the asset does not exist, returns false.
--  If the command can be successfully set, returns true.
--
--  @param[in] upgrade - type string; name of the upgrade i.e. WeaponUpgrade2
--  @return true if command is successfully set, else false
function BuildUpgrade(upgrade)
    debugprint("BuildUpgrade")
    asset = GetSingleAssetWithCapability(AIPointer, upgrade)
    -- debugprint("One" .. asset)
    if asset~=-1 and GetCanApplyCapability(AIPointer, asset, asset, upgrade) then
        debugprint("Two")
        X,Y = GetAssetTilePosition(asset)
        debugprint("Three")
        GiveActorAction(asset, upgrade, X,Y)
        debugprint("Four")
        PushBackCommand(AIPointer, CmdPointer)
        debugprint("Five")
        return true
    end
    debugprint("cannot research")
    return false

end

--- returns the position of the townhall
-- @param TownhallNumber - which townhall u want (first, second, third, etc)
function getTownhallPosition(TownhallNumber)
    TH = TownhallNumber
    TownhallID,TownhallX,TownhallY =   GetTownHallPos(AIPointer);
    return TownhallID,TownhallX,TownhallY
end

--[[
function getFarmPosition()
    --return the nearest wood tile position

    FarmX=10
    FarmY=50
    return FarmX,FarmY
end
]]

--- Commands an unused builder unit to construct a building of type other than town hall. The
-- default structure is a farmhouse, though the type of structure passed in can be changed
--
-- @param[in] buildingType Enum value that dictates what kind of building is constructed
-- @param[in] nearType Enum value that dictates what type of asset this building must be constructed next to
-- @return true if any unit successfully complete this command, or if search map returns true
-- (calls search map returning true)
function BuildBuildings(buildingType)
    debugprint("BuildBuilding: " .. buildingType)
    buildingTypeToActionType = {
        ["TownHall"]="TownHall",
        ["Keep"]="BuildKeep",
        ["Castle"]="BuildCastle",
        ["Farm"]="BuildFarm",
        ["Barracks"]="BuildBarracks",
        ["LumberMill"]="BuildLumberMill",   
        ["Blacksmith"]="BuildBlacksmith",   
        ["ScoutTower"]="BuildScoutTower",
        ["OffensiveTower"]="BuildScoutTower",
        ["OffensiveBarrack"]="BuildBarracks"
    }

    -- print("btype: " .. buildingType)
--  Special Building Handlers
    if(buildingType=="TownHall") then
        return BuildTownHall()
    end
    if(buildingType=="LumberMill") then
        return BuildLumberMill();
    end
    if(buildingType=="OffensiveTower") then
        return OffensiveTower()
    end
    if(buildingType=="OffensiveBarrack") then
        return OffensiveBarrack();
    end


-- Gneral Building Handlers
    debugprint("here")
    buildAction = buildingTypeToActionType[buildingType]
--    BuilderAsset = GetSingleAssetWithCapability(AIPointer, buildAction )
    -- debugprint("Build Asset Set: " .. buildAction)
    -- debugprint("Build Asset Set: " .. buildingType)

    BuilderAsset =  GetBuilder(AIPointer,buildAction, buildingType)
    --debugprint("BuildBuilding " .. buildingType .. nearType .. buildAction)
    --BuilderAsset, PlacementX, PlacementY = BuildBuilding(AIPointer,buildAction, buildingType, nearType)
    debugprint("Build Asset Set: " .. BuilderAsset)    
    if BuilderAsset == -1 then
        --no available builder
        return false
    end
    debugprint("Build Asset Set: " .. BuilderAsset)
    TH_ID, TH_X,TH_Y = getTownhallPosition(1)
    if(TH_ID==-1) then
        --no available townhall
        return false
    end

    if(buildingType=="Farm") then
        PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,builder,{1,0,1},{1,0,1},TH_X,TH_Y,5,1,"Farm",20)
        if(PlacementX==-1) then
            PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,builder,{1,1,1},{1,1,1},TH_X,TH_Y,10,0,"Farm",1000)
        end
    end

    if(buildingType=="Barracks") then
        --num = GetPlayerAssetCount(AIPointer,"Barracks")

        --num_of_Defensive_Barracks = 2
        --change this setting to allow more or less barracks be built next to townhall
        --if num<num_of_Defensive_Barracks then
            PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,builder,{1,1,1},{1,1,1},TH_X,TH_Y,7,2,"Barracks",40)
        if(PlacementX==-1) then
            PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,builder,{1,1,1},{1,1,1},TH_X,TH_Y,0,1,"Barracks",1000)
        end
        --else
        --    return OffensiveBarrack();
        --end
    end
    if(buildingType=="Blacksmith") then
	PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,builder,{1,1,1},{1,1,1},TH_X,TH_Y,7,0,"Barracks",1000)
        if(PlacementX==-1) then
            PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,builder,{1,1,1},{1,1,1},TH_X,TH_Y,0,0,"Blacksmith",40)
	end
    end

    if(buildingType=="ScoutTower") then
        PlacementX,PlacementY = findBestPlacementWithConstraints(AIPointer,builder,{1,0,1},{1,0,1},TH_X,TH_Y,10,1,"ScoutTower",40)
    end



    CanBuild = CanInitiate(AIPointer, BuilderAsset, buildAction)
    debugprint("Build Action can be applied = ")
    debugprint(CanBuild)
    debugprint(PlacementX .. " " .. PlacementY)
    if(0 > PlacementX) then
        return SearchMap()
    end
    if(CanBuild) then
        --debugprint("Giving Action")
        GiveActorAction(BuilderAsset, buildAction, PlacementX, PlacementY)
        PushBackCommand(AIPointer, CmdPointer)
        return true
    end
    return false
end
--- Commands assets with the ability to build footmen to do so, i.e. commands the barracks to
--  begin constructing footmen
--  @return True if any of the AI's units can successfully complete this command
function TrainFootmen()
    debugprint("TrainFootmen")
    assetTable = {}
    assetTable = ConcatTables(assetTable, GetLocationsOfAssetsOfType(AIPointer, "Barracks"))
    -- debugprint("Asset Table Created" .. #assetTable)
	toBuild = "BuildFootman";
	for k = 1, #assetTable, 3 do
	    trainerAsset = assetTable[k]
	    -- debugprint("trainer " .. trainerAsset)
	    canApply = GetCanApplyCapability(AIPointer, trainerAsset, trainerAsset, "BuildFootman")
	    
        if not canApply then
            -- debugprint("try knight")
	    	canApply = GetCanApplyCapability(AIPointer, trainerAsset, trainerAsset, "BuildKnight")
		    -- toBuild = "BuildKnight";
        end
        debugprint(canApply)

	    if canApply then
		-- debugprint("CanApply")
		trainerX, trainerY = GetAssetTilePosition(trainerAsset)
		GiveActorAction(trainerAsset, toBuild, trainerX, trainerY)
		PushBackCommand(AIPointer, CmdPointer)
		return true
	    end
	end
    return false
end

--- Commands some townhall/keep/castle to train a peasant.
--  @return true if successsful, false is no avalible townhall/keep/castle
function TrainPeasant()
    debugprint("TrainPeasant")
    trainerAsset = GetSingleAssetWithCapability(AIPointer, "BuildPeasant")

    if( trainerAsset ~= -1) then
        canApply = GetCanApplyCapability(AIPointer, trainerAsset, trainerAsset, "BuildPeasant")

        if canApply then
            trainerX, trainerY = GetAssetTilePosition(trainerAsset)
            GiveActorAction(trainerAsset, "BuildPeasant", trainerX, trainerY)
            PushBackCommand(AIPointer, CmdPointer)
            return true
        end
    end
    return false
end

--- Commands assets with the ability to build archers to do so, i.e. commands the barracks to
--  begin constructing archers. If the AI player has decided to upgrade archers into rangers,
--  then the barracks will begin constructing rangers instead
--  @return True if any of the AI's units can successfully complete this command

function TrainArcher()
    debugprint("TrainArcher")
    assetTable = {}
    assetTable = ConcatTables(assetTable, GetLocationsOfAssetsOfType(AIPointer, "Barracks"))
    
    debugprint("Asset Table Created")
	toBuild = "BuildArcher";
	for k = 1, #assetTable, 3 do
	    trainerAsset = assetTable[k]
	    -- debugprint("trainer" .. trainerAsset)
	    canApply = GetCanApplyCapability(AIPointer, trainerAsset, trainerAsset, "BuildArcher")
	    
	    if not canApply then
	    	canApply = GetCanApplyCapability(AIPointer, trainerAsset, trainerAsset, "BuildRanger")
		    toBuild = "BuildRanger";
	    end
	    if canApply then
		-- debugprint("CanApply")
		trainerX, trainerY = GetAssetTilePosition(trainerAsset)
		GiveActorAction(trainerAsset, toBuild, trainerX, trainerY)
		PushBackCommand(AIPointer, CmdPointer)
		return true
	    end
	end
    return false
end

--- Commands assets with the ability to standground to do so, if they are not already standing ground.
-- @return true if any AI units can successfully standground
function ActivateFighters()
    debugprint("ActivateFighters")
   size = GetAllIdleAssetsWithCapability(AIPointer, CmdPointer, "StandGround", "StandGround")   
   if size > 0 then
        SetCommandAction(AIPointer,CmdPointer, "StandGround")
        PushBackCommand(AIPointer, CmdPointer)
        return true
   end
   return false
end

--- Commands assets that are fighters to attack the nearest enemy.
--  The function calculates the average position of all units, and uses that point to determine the nearest enemy.
--  @return true if a command is set, otherwise false.
function AttackEnemies(commandsAllocated)
    debugprint("AttackEnemies")

    locationTable = GetAllPlayerAssetsWithCapability(AIPointer, "Attack","Patrol")
    units, clusters = UnitKMeans(locationTable, commandsAllocated)
    clusterActors = {}
    for i = 1, #clusters, 1 do
        clusterActors[i] = {}
    end

    for i = 1, #units, 1 do
        clusterActors[units[i][4]][#clusterActors[units[i][4]] + 1] = units[i][1]
    end
    assignments = 0
    for i = 1, #clusters, 1 do
        averageX = clusters[i][1]
        averageY = clusters[i][2]
        targetAsset = GetNearestEnemy(AIPointer, averageX, averageY, -1)
        if targetAsset == -1 then
            if SearchMap() then 
                return 1
            else
                return 0
            end
        else
            targetX, targetY = GetAssetTilePosition(targetAsset)
            targetColor = GetAssetColor(targetAsset)

            for j = 1, #clusterActors[i], 1 do
                AddCommandActor(AIPointer, CmdPointer, clusterActors[i][j])
            end
            targetType = GetAssetType(targetAsset)
            SetCommandAction(AIPointer, CmdPointer, "Attack")   
            SetCommandTargetPos(AIPointer, CmdPointer, targetX, targetY)
            SetCommandTargetColor(AIPointer, CmdPointer, targetColor)
            SetCommandTargetType(AIPointer, CmdPointer, targetType)
            PushBackCommand(AIPointer, CmdPointer)
            assignments = assignments + 1
        end
    end
    return assignments
end

function UnitKMeans(locations, clusterAmt)
    --Create Data Points
    points = {}
	point = 1
    for i = 1, #locations, 3 do
        points[point] = {locations[i], locations[i + 1], locations[1 + 2], -1}
		point = point + 1
    end
	
    clusters = {}
    amt = 0
    for i = 1, clusterAmt, 1 do
        if i <= #points then
            clusters[i] = {points[i][2], points[i][3]}
        else
            break
        end
    end

    while true do
        --Assign Points to Clusters
        changed = false
        for i = 1, #points, 1 do
            minDist = -1
			oldCluster = points[i][4]
            for j = 1, #clusters, 1 do
                dx = points[i][2] - clusters[j][1]
                dy = points[i][3] - clusters[j][2]
                dist = dx * dx + dy * dy
                if dist < minDist or minDist == -1 then
                    minDist = dist
                    points[i][4] = j
                end
            end
			if points[i][4] ~= oldCluster then
				changed = true
			end
        end

        --Calculate Cluster Centroids
        clustercalc = {}
        for i = 1, #clusters, 1 do
            clusters[i] = {0, 0}
            clustercalc[i] = {}
        end
        for i = 1, #points, 1 do
            clustercalc[points[i][4]][#clustercalc[points[i][4]] + 1] = {points[i][2], points[i][3]}
        end
        for i = 1, #clustercalc, 1 do
            for j = 1, #clustercalc[i], 1 do
                clusters[i][1] = clusters[i][1] + clustercalc[i][j][1] / #clustercalc[i]
                clusters[i][2] = clusters[i][2] + clustercalc[i][j][2] / #clustercalc[i]
            end
        end

        --Break if no changes
        if not changed then
            break
        end
    end

    return points, clusters
end

function BuildWalls (numBuilders, buildRadius)
    debugprint("BuildWalls with radius " .. buildRadius)
    potentialBuilders = GetAllPlayerAssetsWithCapability(AIPointer, "BuildWall", "BuildWall")
    buildingTable = GetAllPlayerBuildingAssets(AIPointer)

    debugprint("    Retrieved Tables")
    if #buildingTable < 3 or #potentialBuilders < 3 then
        return 0
    end

    debugprint("      #buildingTable = " .. #buildingTable .. ", #potentialBuilders = " .. #potentialBuilders)
    for i = 1, #buildingTable, 3 do
        debugprint("        Building " .. (i - 1) / 3 + 1 .. " at " .. buildingTable[i + 1] .. ", " .. buildingTable[i + 2])
    end

    minX = buildingTable[2] - buildRadius
    maxX = buildingTable[2] + buildRadius
    minY = buildingTable[3] - buildRadius
    maxY = buildingTable[3] + buildRadius
    for i = 4, #buildingTable, 3 do
        if buildingTable[i + 1] - buildRadius < minX then
            minX = buildingTable[i + 1] - buildRadius
        end
        if buildingTable[i + 1] + buildRadius > maxX then
            maxX = buildingTable[i + 1] + buildRadius
        end
        if buildingTable[i + 2] - buildRadius < minY then
            minY = buildingTable[i + 2] - buildRadius
        end
        if buildingTable[i + 2] + buildRadius > maxY then
            maxY = buildingTable[i + 2] + buildRadius
        end
    end

    debugprint("    Mins and maxes calculated (" .. minX .. ", " .. minY .. ") to (" .. maxX .. ", " .. maxY ..")")

    noBuildZone = {}
    sizeX = maxX - minX + 1
    sizeY = maxY - minY + 1
    debugprint("      Sizes X:" .. sizeX .. ", Y:" .. sizeY)
    for i = 1, sizeX, 1 do
        noBuildZone[i] = {}
        for j = 1, sizeY, 1 do
            noBuildZone[i][j] = 0
        end
    end

    debugprint("    noBuildZone initialized")

    for i = 1, #buildingTable, 3 do
        for m = -1 * buildRadius, buildRadius, 1 do
            for n = -1 * buildRadius, buildRadius, 1 do
                noBuildZone[buildingTable[i + 1] - minX + 1 + m][buildingTable[i + 2] - minY + 1 + n] = 1
            end
        end
    end

    debugprint("    noBuildZone Set")

    wallPlacement = {}
    for i = 1, sizeX, 1 do
        for j = 1, sizeY, 1 do
            if noBuildZone[i][j] == 1 then
                if i == 1 or i == sizeX or j == 1 or j == sizeY then
                    wallPlacement[#wallPlacement + 1] = i + minX
                    wallPlacement[#wallPlacement + 1] = j + minY
                else
                    wallFound = false
                    for m = -1, 1, 1 do
                        for n = -1, 1, 1 do
                            if i + m >= 1 and i + m <= sizeX and j + n >= 1 and j + n <= sizeY then
                                if noBuildZone[i + m][j + n] == 0 then
                                    wallPlacement[#wallPlacement + 1] = i + minX
                                    wallPlacement[#wallPlacement + 1] = j + minY
                                    wallFound = true
                                    break
                                end
                            end
                        end
                        if wallFound then
                            break
                        end
                    end
                end
            end
        end
    end

    debugprint("    wallPlacecments found")

    wallToBuild = 1
    for builder = 1, numBuilders, 1 do
        if (builder - 1) * 3 + 1 > #potentialBuilders then
            return builder - 1
        end
        for i = wallToBuild, #wallPlacement, 2 do
            wallToBuild = wallToBuild + 2
            canBuild = GetCanPlaceAssetOfSize(AIPointer, 1, potentialBuilders[(builder - 1) * 3 + 1], wallPlacement[i], wallPlacement[i + 1])
            if canBuild then
                GiveActorAction(potentialBuilders[(builder - 1) * 3 + 1], "BuildWall", wallPlacement[i], wallPlacement[i + 1])
                PushBackCommand(AIPointer, CmdPointer)
                break
            end
        end
    end
    return numBuilders
end

--========================== Set Command Macros ===============================================


--- Sets command to an action on a single actor on neutral target
-- @param y y coordinate
-- @param x x coordinate
-- @param action capability to act on
-- @param actorID ID of asset given build action
function GiveActorAction (actorID, action, x, y)
    debugprint("Giving Actor Action")
    SetCommandAction(AIPointer, CmdPointer, action)
    debugprint("    SetAction ".. action)
    debugprint("    AddedActor " .. actorID)


    AddCommandActor(AIPointer, CmdPointer, actorID)



    debugprint("    AddedActor " .. actorID)
    SetCommandTargetPos(AIPointer, CmdPointer, x, y)
    debugprint("    SetTargetPos " .. x .. ", " .. y)
end

--- Sets command to an action on all pre-selected actor on neutral target
-- @param y y coordinate
-- @param x x coordinate
-- @param action capability to act on
function GiveActorsAction (action, x, y)
    SetCommandAction(AIPointer, CmdPointer, action)
    SetCommandTargetPos(AIPointer, CmdPointer, x, y)
end

--- Sets command to tell a given peasant to mine
-- @param mineY the y coordinate of the mine
-- @param mineX the x coordinate of the mine
-- @param actorID the AssetID of the peasant to mine
function MineGold (actorID, mineX, mineY)
    SetCommandAction(AIPointer, CmdPointer, "Mine")
    AddCommandActor(AIPointer, CmdPointer, actorID)
    SetCommandTargetType(AIPointer, CmdPointer, "GoldMine")
    SetCommandTargetPos(AIPointer, CmdPointer, mineX, mineY)

end

--- Sets command to tell a given peasant to convey resources to a town hall
-- @param townHallY the y coordinate of the town hall
-- @param townHallY the x coordinate of the town hall
-- @param townHallAssetID the AssetID of the town hall to return to
-- @param actorID the AssetID of the peasant to convey
function ConveyResources (actorID, townHallAssetID, townHallX, townHallY)
    -- print(actorID .. " "..  townHallAssetID .. " "..  townHallX .. " "..  townHallY .. " "..  AIColor .. " " .. GetAssetType(townHallAssetID))
    SetCommandAction(AIPointer, CmdPointer, "Convey")
    AddCommandActor(AIPointer, CmdPointer, actorID)
    SetCommandTargetColor(AIPointer, CmdPointer, AIColor)
    SetCommandTargetType(AIPointer, CmdPointer, GetAssetType(townHallAssetID))
    SetCommandTargetPos(AIPointer, CmdPointer, townHallX, townHallY)
end

---------------------------- Helper Functions ------------------------------------

--- Concatenates two tables t1 and t2
--  @param t1 - a lua table
--  @param t2 - a lua table
--  @return lua_table containing all elements in t1 followed by elements in t2
function ConcatTables (t1, t2)
    for i = 1, #t2 do
        t1[#t1 + 1] = t2[i]
    end
    return t1
end
