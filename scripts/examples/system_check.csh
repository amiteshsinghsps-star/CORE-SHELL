#!/usr/bin/coreshell
# Example CoreShell script: system_check.csh

LOG_FILE="/tmp/syscheck.log"
DATE=$(date +%Y-%m-%d)

check_disk() {
    USAGE=$(df -h / | awk 'NR==2 {print $5}')
    echo "Disk usage: $USAGE"
}

if [ $(id -u) -eq 0 ]; then
    echo "Running as root"
else
    echo "Running as user: $USER"
fi

for proc in nginx apache2 sshd; do
    if pgrep -x "$proc" > /dev/null; then
        echo "$proc is running" >> $LOG_FILE
    fi
done

COUNT=0
while [ $COUNT -lt 5 ]; do
    check_disk
    COUNT=$((COUNT + 1))
    sleep 1
done
