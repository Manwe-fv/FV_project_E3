|----------------------------------------------------------------------------|
| Druid Functions
|----------------------------------------------------------------------------|
Sub DRU_Setup
	/call iniToVarV "${Character_Ini},Druid,Evac Spell" Evac_Spell string outer
  /call IniToArrayV "${Character_Ini},Druid,Charm#"	charmArray
  
  /declare charmTarget int outer
  /if (${Bool[${charmArray.Size}]}) {
    /call BuildSpellArray "charmArray"	"charmArray2D"
    /declare doCharm bool outer TRUE
    /declare charmBuffTimer timer outer
  } else {
    /declare doCharm bool outer FALSE
  }
/return
|----------------------------------------------------------------------------|
SUB DRU_MacroSettings
/if (${Debug}) /echo |- DRU_MacroSettings ==>
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#1" "check_healChain" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#2" "check_Heals" 1
  /call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#3" "check_Cures" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#4" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#5" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#6" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#7" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#8" "check_AE" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#9" "check_Nukes" 1
/RETURN
|----------------------------------------------------------------------------|
SUB DRU_CharacterSettings
	/call WriteToIni "${Character_Ini},Druid,Evac Spell"
    /call WriteToIni "${Character_Ini},Druid,Charm"
/RETURN
|----------------------------------------------------------------------------|
Sub DRU_Aliases
/return
|----------------------------------------------------------------------------|
#event AE_SOW "#*# tells you, 'AESOW'"
sub Event_AE_SOW(string line)
	/if (${Me.Class.ShortName.Equal[DRU]}) {
	 /if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[Spirit of the Wood]}) {
		 /bc MGB Spirit of the Wood inc...
		 /casting "Mass Group Buff|alt" -maxtries|3
		 /delay 5 !${Me.AltAbilityReady[Mass Group Buff]}
		 /delay 5
		 /casting "Spirit of the Wood|alt" -maxtries|3
		 /delay 5 !${Me.AltAbilityReady[Spirit of the Wood]}
		 /casting "Spirit of the Wood|alt" -maxtries|3
		 /rs MGB Spirit of the Wood inc...
	 } else /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
			/bc Mass Group Buff is not available...
	 } else /bc Spirit of the Wood is not available...
	}
/return

#event AE_FOE "#*# tells you, 'AEFOE'"
sub Event_AE_FOE(string line)
	/if (${Me.Class.ShortName.Equal[DRU]}) {
	 /if (${Me.AltAbilityReady[Mass Group Buff]} && ${Bool[${Me.Book[Flight of Eagles]}]}) {
		 /bc MGB Flight of Eagles inc...
		 /casting "Mass Group Buff|alt" -maxtries|3
		 /delay 5 !${Me.AltAbilityReady[Mass Group Buff]}
		 /delay 5
		 /casting "Flight of Eagles" -maxtries|3
		 /delay 5 !${Me.AltAbilityReady[Spirit of the Wood]}
		 /casting "Flight of Eagles" -maxtries|3
		 /rs MGB Spirit of the Wood inc...
	 } else /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
			/bc Mass Group Buff is not available...
	 } else /bc Flight of Eagles is not available...
	}
/return
|----------------------------------------------------------------------------|
#event druid_charmOn "<#1#> Charm on #2#"
#event druid_charmOn "#1# tells you, 'Charm on #2#'"
sub event_druid_charmOn(line, ChatSender, targetID)
  /if (${Me.Class.ShortName.Equal[DRU]}) {
    /if (${Debug} || ${Debug_Ench}) /echo |-  charmOn ==>
    |&& (!${Spawn[id ${targetID}].Named} || ${Spawn[id ${targetID}].CleanName.Equal[Coerced Lieutenant]})
      /if ((${Spawn[id ${targetID}].Distance} < 240 && ${Spawn[id ${targetID}].Level} <= ${Me.Level} && ${SpawnCount[npc id ${targetID}]}>0) && ${Select[${Spawn[id ${targetID}].Body},Animal,Insect,Plant]} || ${targetID}==${Me.Pet.ID}) {
        /varset charmTarget ${targetID}
        /varset doCharm TRUE
        /docommand ${ChatToggle} << Charm enabled ${Spawn[id ${charmTarget}].CleanName} >>
      } else {
        /docommand ${ChatToggle} << ${Spawn[id ${targetID}].CleanName} is not eligible for charm >>
      }
    /if (${Debug}) /echo <== charmOn -|
  }
/return
|----------------------------------------------------------------------------|
#event druid_charmOff "<#1#> Charm Off"
#event druid_charmOff "#1# tells you, 'Charm Off'"
sub event_druid_charmOff(line, ChatSender)
/if (${Debug} || ${Debug_Ench}) /echo |- charmOff ==>
  /if (${Me.Class.ShortName.Equal[DRU]}) {
      /if (${doCharm}) {
        /docommand ${ChatToggle} << Charm Disabled ${Spawn[id ${charmTarget}].CleanName} >>
        /varset doCharm FALSE
        /varset charmTarget 0
      }
    /if (${Debug}) /echo <== charmOff -|
  }
/return
|----------------------------------------------------------------------------|
Sub check_charm
/if (${Debug} || ${Debug_Ench}) /echo |- check_Charm ==>
/if (${Defined[charmArray2D]}) {
  /declare s int local
  /declare charmDuration int local
  /if (${doCharm} && ${Bool[${charmTarget}]}) {
    /if (${SpawnCount[npc id ${charmTarget}]}==0 && ${charmTarget}!=${Me.Pet.ID}) {
      /docommand ${ChatToggle} Disabling Charm: ${charmTarget} is no longer a valid npc
      /varset doCharm FALSE
      /varset charmTarget 0
      /return
    }
    /if (${Me.Pet.Race.Name.Equal[Invisible Man]}) {
      /pet get lost
      /delay 1s !${Me.Pet.ID}
    }
    /if (!${Bool[${Me.Pet.ID}]}) {
      /bc [+r+] << ${Spawn[id ${charmTarget}].CleanName} is not Charmed! >> [+r+]
      /for s 1 to ${charmArray2D.Size[1]}
      /call check_Ready "charmArray2D" ${s}
      /if (${c_Ready}) {
        /if (${check_Mana["charmArray2D",${s}]}) {
          /if (${check_Distance[${charmTarget},${charmArray2D[${s},${iMyRange}]}]}) {
            /docommand ${ChatToggle} << Charming ${charmTarget} ${Spawn[id ${charmTarget}].CleanName} with ${charmArray2D[${s},${iCastName}]} >>
            /call e3_Cast ${charmTarget} "charmArray2D" "${s}"
            /if (${Bool[${Me.Pet.ID}]}) {
              /bc [+g+] >> Charmed ${Spawn[id ${charmTarget}].CleanName} <<
              /timed 5 /pet back off
              /timed 10 /pet follow
            } else {
              /if (${castReturn.Equal[CAST_IMMUNE]}) {
                /varset doCharm FALSE
                /varset charmTarget 0
                /docommand ${ChatToggle} << ${Spawn[id ${charmTarget}].CleanName} is immune, turning off charm >>
              }
            }
          }
        }
      }
      /if (!${Bool[${Me.Pet.ID}]}) /next s
    } else {
    |accounce time left on charm
      /for s 1 to ${charmArray2D.Size[1]}
        /if (${Bool[${Me.PetBuff[${charmArray2D[${s},${iSpellName}]}]}]}) {
          /varset charmDuration ${Pet.BuffDuration[${charmArray2D[${s},${iSpellName}]}].TotalSeconds}
          |/echo charmduration ${charmDuration} timer ${charmBuffTimer}
          /if (${charmDuration} > 60) {
            /if (!${Bool[${charmBuffTimer}]}) {
              /bc [+g+] My charm fades in ${charmDuration} sec
              /varset charmBuffTimer 18s
            }
          } else /if (${charmDuration} <= 60 && ${charmDuration} > 9) {
            /if (!${Bool[${charmBuffTimer}]}) {
              /bc [+y+] My charm fades in ${charmDuration} sec
              /varset charmBuffTimer 6s
            }
          } else /if (${charmDuration} <= 9) {
            /if (!${Bool[${charmBuffTimer}]}) {
              /bc [+r+] My charm fades in ${charmDuration} sec
              /varset charmBuffTimer 1s
              }
          }
        }
      /next s
    }
  }
}
/if (${Debug} || ${Debug_Ench}) /echo <== check_Charm -|
/return
|----------------------------------------------------------------------------|

Sub DRU_Background_Events
	/doevents AE_SOW
	/doevents AE_FOE
    /doevents charmTarget
    /doevents druid_charmOn
    /doevents druid_charmOff
/return
