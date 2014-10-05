#!/usr/bin/env python

### global var
# Mark whether server is still running, for sock thread
running = True

# Format of socklist {uid:[sockfd, clientstatus], ...}
socklist = {}

# For ui to update 
# Format [uid, 'online'/'offline']
updateinfo = []

