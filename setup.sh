#!/bin/bash
rm sendMail.sh
echo "Please enter the email of your emergency contact: "
read inputline
printf "echo \"I fell from my bike! Please send help!\" | mail -s \"HELP\" -A \"photo.jpg\" $inputline\necho \"Email sent!\"" >> sendMail.sh
chmod 755 sendMail.sh
echo "Emergency contact set as \"$inputline\""

