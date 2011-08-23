#!/usr/bin/env tclsh

# Patched version of cgos3.tcl (as downloaded on 2007-10-29 from
# http://cgos.boardspace.net) to inform the player about the opponent's name
# and game result using the command go_set_info used by fuego/go/GoGtpEngine.
# Patched lines are marked with the comment PATCHED

set server cgos.boardspace.net
set port   6819



# Make older tcl versions compatible with 8.5 by
# defining the lassign proc if it doesn't exist
# ------------------------------------------------------
if {[info procs lassign] eq ""} {
    proc lassign {values args} {
	uplevel 1 [list foreach $args [linsert $values end {}] break]
	lrange $values [llength $args] end
    }
}




# -----------------------------------------------------
# timeout in seconds 
# Don't make this too low - no need to hammer the server
# 
#   When CGOS is restarted it will always wait 45 seconds
#   before scheduling the first round - to allow all the
#   clients time to log back on.   So 30 seconds is
#   a good suggested value for timeout.
# ------------------------------------------------------
set server_retry_timeout  30    



proc platform {} {
    global tcl_platform
    set plat [lindex $tcl_platform(os) 0]
    set mach $tcl_platform(machine)
    switch -glob -- $mach {
	sun4* { set mach sparc }
	intel -
	i*86* { set mach x86 }
	"Power Macintosh" { set mach ppc }
    }
    switch -- $plat {
	AIX   { set mach ppc }
	HP-UX { set mach hppa }
    }
    return "$plat-$mach"
}



# -----------------------------------------------------------
# each client should send some string to identify it
# should not have line feeds and is trucated to 60 characters
# -----------------------------------------------------------
set clientInfo "CGOS tcl engine client 1.2 [platform] by Don Dailey"



# todo: should have proper command line parsing and switches
# ----------------------------------------------------------
if {[llength $argv] < 3} {

    puts stderr $clientInfo

    puts stderr "Usage: cgos3.tcl  NAME PASSWORD INVOCATION  {SENTINEL_FILE}\n"
    exit 1
}


proc log {msg} {
    # set tme [clock format [clock seconds] -format "%Y-%m-%d %H:%M:%S" -timezone :UTC]
    set tme [clock format [clock seconds] -format "%H:%M:%S"]
    puts  "$tme    $msg" 
}



set pname [lindex $argv 0]
set pass  [lindex $argv 1]
set invoke [lindex $argv 2]

set sentinel_file [lindex $argv 3]
set use_sentinel  [string length $sentinel_file]


set time_out [expr $server_retry_timeout * 1000]


set inout [open "|$invoke" r+]

set progpid [pid $inout]

fconfigure $inout -buffering line


log  "C->E list_commands"
puts $inout "list_commands"

while { [gets $inout s] } { 

    if {[regexp {\=\s+(\S+)} $s dmy val]} {
	set cmd $val
    } else {
	set cmd $s
    }

    log  "E->C $cmd"
    set known($cmd) 1
    if {$cmd == ""} { break }
} 

log "recieved full response to list_commands"


# determine if engine supports gtp timing commands
# ------------------------------------------------
set use_time 0

if {[info exists known(time_left)] && [info exists known(time_settings)]} {
    set use_time 1
}

if {$use_time} {
    log  "Engine uses  time control commands"
} else {
    log  "Engine does NOT use time control commands"
}



# remove old sentinel file if it exists
# -------------------------------------
if {$use_sentinel} {

    # Not an error to try to delete a non-existent file in tcl
    # --------------------------------------------------------
    if {[file exists $sentinel_file]} {
	file delete $sentinel_file
    }
}


# ---------------------------------
# send a message and get a response 
# from channel ch
# ---------------------------------
proc  eCmd { ch msg } {
    global  progpid

    log  "C->E $msg"
    puts $ch $msg
    gets $ch r1
    gets $ch r2

    log "E->C $r1"

    if { [string index $r1 0] == "?" } {
	log "Error. Engine returns error error response.  Aborting"
	exit 1
    }

    return $r1
}


# send a message to the cgos server
# ---------------------------------
proc cgos {msg} {
    global   sock

    log "C->S $msg"
    puts $sock $msg
}




# setup server restart loop
# -------------------------
while {1} {

    set err [catch {set sock [socket $server $port]} msg]

    if { $err } {
	log "Server startup return code: $err   msg: $msg"
	log "Cannot connect to server.  Will retry shortly"
	after $time_out  ;# Wait several seconds 
	continue
    } else {
	log "Successful connection to CGOS server"
    }

    fconfigure $sock -buffering line
    
    while {[gets $sock s]} {
	
	if { [regexp {^(\S+)} $s dmy cmd] } {

	    log "S->C $s"
	    
	    switch -exact $cmd {
		
		info { 
		    # informational mssage from CGOS only
		    # -----------------------------------
		    log [string range $s 5 end] 
		}  
		
		protocol { cgos "e1 $clientInfo" }
		username { cgos $pname }
		password { cgos $pass  }
		Error:   { break }

		setup {
		    set mvlist [lassign $s ignore gid boardsize komi timeLeft white black]

		    set msg [eCmd $inout "boardsize $boardsize"]
		    set msg [eCmd $inout "clear_board"]
		    set msg [eCmd $inout "komi $komi"]
		    
		    if {$use_time} {
			set  tme [expr $timeLeft / 1000]
			set msg [eCmd $inout "time_settings $tme 0 0"]
		    }

		    # PATCHED
		    set msg [eCmd $inout "go_set_info player_black $black"]
		    # PATCHED
		    set msg [eCmd $inout "go_set_info player_white $white"]
		    # PATCHED
		    set msg [eCmd $inout "go_set_info game_name $gid"]

		    # catch up game if needed
		    # -----------------------
		    set ctm B 
		    foreach {mv tme} $mvlist {
			set msg [eCmd $inout "play $ctm $mv"]
			if {$ctm == "B"} { set ctm W } else { set ctm B }
		    }

		    # setup requires no response - we are done
		}


		genmove {
		    lassign $s dmy col remaining_time

		    if {$use_time} {
			set tme [expr $remaining_time / 1000]
			set msg [eCmd $inout "time_left $col $tme 0"]
		    }
		    
		    set msg [eCmd $inout "genmove $col"]

		    regexp {=\s+(\S+)} $msg dmy mv
		    cgos $mv
		}

		
		gameover {
		    # PATCHED
		    #lassign $s res dte err
		    lassign $s ignore dte res

		    # PATCHED
		    set msg [eCmd $inout "go_set_info result $res"]

		    if {$use_sentinel} {
			if {[file exists $sentinel_file]} {
			    # user wants to exit the server
			    log "Sentinal file detected.  Aborting"
			    puts $inout quit  ;# shutdown engine (and don't expect a response)
			    exit 0
			}
		    }
		    cgos "ready"    ;# tell server we want to play another game
		}
		

		play {
		    lassign $s dmy col mv tme

		    # some engines do not understand "resign"
		    if { $mv != "resign" } {
			set msg [eCmd $inout "play $col $mv"]
		    }
		}

		
		default { 
		    log  "We do not understand server request."
		    log  "Perhaps you need to update your client?"
		    log  "Exit"
		    eCmd $inout "quit"
		    exit 1
		}
	    }
	} else {
	    log "Irrecgular response from server. Breaking connection."
	    break  ;# something funny here from server.
	}
	
    }


    # presumably, socket has closed on us at this point
    # We should now wait a few minutes and try to reconnect

    # Go program should still be running.  todo: check that it is
    # -----------------------------------------------------------

    log  "Connection to server has closed.  Will try to reconnect shortly." 
    after $time_out  ;# Wait a few seconds 
}
