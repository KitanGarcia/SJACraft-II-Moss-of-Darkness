--- Ends the game with a win or lose state
function EndGame (won, forced)
    EndGame_CPP(won == "1", forced == "1")
end

--- Increases Gold/Lumber/Stone by amount/ or decreases if negative
--  @param amount - amount to add/subtract
--  @param typeR  - reource type (gold, lumber, stone)
function ChangeResources (amount, typeR)
    a = tonumber(amount)
    ChangeResources_CPP(PlayerColor, a, typeR)
end

--- changes asset health by delta
--  @param delta - amount of health to add or subtract (positive = add, negative = subtract)
function ModifyAssetHealth (delta)
    if OffenderID == -1 then
        return
    end
    d = tonumber (delta)
    ModifyAssetHealth_CPP(OffenderID, d)
end

--- Removes asset if valid asset ID
-- @param - No parameters
function RemoveAsset()
    if OffenderID == -1 then 
        return
    end
    RemoveAsset_CPP(OffenderID)
end

--- Adds an asset of assetType onto the location (PositionX, PositionY)
--  if an asset already exists at this location, it will spawn at the closest avalible spot
-- @param AssetName - name of asset to spawn
-- @param PositionX - Location to spawn X-coord
-- @param PositionY - Location to spawn Y-coord 
-- @param amount - Amount of resources in the resource asset (ignored if not resource )
function AddAsset(AssetName, Targetcolor, PositionX, PositionY, amount)
    AddAsset_CPP(AssetName, Targetcolor, PositionX, PositionY, amount)
end

--- For creating resource assets: GoldMine, Stone Quarry
--  Adds an asset of assetType onto the location (PositionX, PositionY)
--  if an asset already exists at this location, it will spawn at the closest avalible spot
-- @param AssetName - name of asset to spawn
-- @param PositionX - Location to spawn X-coord
-- @param PositionY - Location to spawn Y-coord 
function AddResourceAsset(AssetName, Targetcolor, PositionX, PositionY, amount)
    AddResourceAsset_CPP(AssetName, Targetcolor, PositionX, PositionY)
end

--- Adds an upgrade to designated player color's assets
--  @param Targetcolor - Color of the target to give the upgrade (i.e Red, Blue, etc)
--  @param upgrade     - name of the upgrade (i.e WeaponUpgrade2)
function AddUpgrade(Targetcolor, upgrade)
    AddUpgrade_CPP(Targetcolor, upgrade)
end

--- Removes an upgrade to designated player color's assets
--  @param Targetcolor - Color of the target to give the upgrade (i.e Red, Blue, etc)
--  @param upgrade     - name of the upgrade (i.e WeaponUpgrade2)
function RemoveUpgrade(Targetcolor, upgrade)
    RemoveUpgrade_CPP(Targetcolor, upgrade)
end
    
--- Kills a random unit owned by a certain player
function KillRandPlayerUnit()
    unitTable = GetPlayerUnits();
    if #unitTable > 0 then
        toKill = unitTable[math.random(#unitTable)]
        RemoveAsset_CPP(toKill);
    end
end