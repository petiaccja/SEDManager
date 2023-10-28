# SEDManager

SEDManager is a library and application to manage self-encrypting drives (SEDs) that comply with the Trusted Computing Group (TCG) specifications.


## Device support

### TCG storage subsystem classes

| TCG SSC | Status |
|---|---|
| Enterprise | Partial |
| Opal v1 | Full ✔️ |
| Opal v2 | Full ✔️ |
| Opalite | Partial |
| Pyrite v1 | Partial |
| Pyrite v2 | Partial |
| Ruby | Partial |
| Key Per I/O | Detect |

For more information on the SSCs, see https://trustedcomputinggroup.org/work-groups/storage/.

### Bus interfaces

| Interface | Status |
|---|---|
| NVMe | ✔️ |
| ATA | ❌️ |
| SATA | ❌️ |
| SCSI | ❌️ |
| SD | ❌️ |
| MMC | ❌️ |

### Explanation of support statuses

| Status | Explanation |
|---|---|
| Full ✔️ | SEDManager recognizes all tables and preconfiguration data. You can edit all tables, have access to column information, and use friendly names. |
| Partial | SEDManager recognizes all tables, but does not have the preconfiguration data. You can edit all tables, have access to column information, but have to use a mix of UIDs and friendly names. |
| Detect | SEDManager does not recognize all tables, neither has the preconfiguration data. You can edit all tables, but have no access to column information and have to use a mix of UIDs and friendly names. **Limitation:** you cannot edit tables that are not recognized. This should be resolved in the future. |


## Usage

### Modes

There are three modes available:
- **Interactive**: gives you complete freedom over configuring your device, but it's also the most difficult to use.
- **Guided**: provides easy configuration of most common usage scenarios. **This feature is only planned**, there aren't any guided modes available currently.
- **PBA**: scans the system for locked drives, and allows you to unlock them with a username/password combination. Intended for pre-boot authentication environments.

### Before you begin

IMPORTANT: before doing anything to your drive, MAKE SURE THAT YOU ARE ABLE TO PERFORM A [PSID REVERT](#psid_revert). If you lose your password(s), you don't only lose all your data, you won't be able to use your device at all unless you do a PSID revert.

WARNING: several operations result in complete loss of data, most notably reverting the device and generating new media encryption keys. If you want to lock down a drive that's already in use, it's best to back your data up.


### Interactive mode

You can launch an interactive shell like this:

```shell
# Linux
sudo SEDManagerCLI --interactive /dev/nvme0
# Windows
SEDManagerCLI.exe --interactive \\.\PHYSICALDISK0
```

#### Taking ownership of the drive

After you've launched the interactive shell, you should see this:

```
Drive: Samsung SSD 980 PRO 2TB
<no session>>
```

Type `'start Admin'` to start a session on the Admin SP:

```
<no session>> start Admin # Start a session on the Admin SP
Admin[ Anybody ]> # You current SP and authenticated authorities show here
```

Next, query the MSID password, which you can use to authenticate as the drive's owner for the first time:

```
Admin[ Anybody ]> get C_PIN::MSID 3 # C_PIN table, MSID row, 3rd column
"***" # Your MSID password
Admin[ Anybody ]> auth SID # SID is the owner of the drive
Password: # Type your MSID password from above
Admin[ SID ]> # You're now logged in as the owner!
```

Finally, change your SID (owner) password. If you forget this, you'll have to do a [PSID revert](#psid_revert) to be able to use your drive again, which will erase all your data.

```
Admin[ SID ]> passwd SID
New password: ***
Retype password: ***
Admin[ SID ]>
```

You can now end the session and try if you new password works, or you can continue configuring:

```
Admin[ SID ]> end
```

#### Enable locking functionality

This makes locking available, but does not actually lock your drive yet.

```
# Launch a session on the Admin SP
<no session>> start Admin
# Authenticate as the drive owner
Admin[ Anybody ]> auth SID
Password:
# Activate the Locking SP - this enable the locking functionality
Admin[ SID ]> activate SP::Locking
# End the session on the Admin SP and start a new one on the Locking SP
Admin[ SID ]> end
<no session>> start Locking
# Authenticate as Admin1: your password is initially the same as your SID password
Locking[ Anybody ]> auth Admin1
Password:
# You can change your Admin1 password, but it's not necessary.
Locking[ Admin1 ]> passwd Admin1
New password: ***
Retype password: ***
Locking[ Admin1 ]>
```

#### Lock the drive with a password

This procedure does lock your drive down. Your data is encrypted and cannot be accessed unless you provide the right password.

Start a session on the Locking SP and authenticate as Admin1:

```
<no session>> start Locking
Locking[ Anybody ]> auth Admin1
Password: ***
```

Get the list of users supported by the device:

```
Locking[ Admin1 ]> rows Authority
| UID                | Name               |
+--------------------+--------------------+
| 0x0000000900000001 | Authority::Anybody |
| 0x0000000900000002 | Authority::Admins  |
| 0x0000000900010001 | Authority::Admin1  |
| 0x0000000900010002 | Authority::Admin2  |
| 0x0000000900010003 | Authority::Admin3  |
| 0x0000000900010004 | Authority::Admin4  |
| 0x0000000900030000 | Authority::Users   |
| 0x0000000900030001 | Authority::User1   |
| 0x0000000900030002 | Authority::User2   |
| 0x0000000900030003 | Authority::User3   |
| 0x0000000900030004 | Authority::User4   |
| 0x0000000900030005 | Authority::User5   |
| 0x0000000900030006 | Authority::User6   |
| 0x0000000900030007 | Authority::User7   |
| 0x0000000900030008 | Authority::User8   |
| 0x0000000900030009 | Authority::User9   |
```

Get some more information on `User1`:

```
Locking[ Admin1 ]> get Authority::User1
| Column                | Value                         |
+-----------------------+-------------------------------+
| 0: UID                | "00'00'00'09'00'03'00'01"     |
| 1: Name               | <empty>                       |
| 2: CommonName         | ""                            |
| 3: IsClass            | 0                             |
| 4: Class              | "ref:00'00'00'09'00'03'00'00" |
| 5: Enabled            | 0                             |
| 6: Secure             | 0                             |
| 7: HashAndSign        | 0                             |
| 8: PresentCertificate | 0                             |
| 9: Operation          | 1                             |
| 10: Credential        | "ref:00'00'00'0B'00'03'00'01" |
| 11: ResponseSign      | "ref:00'00'00'00'00'00'00'00" |
| 12: ResponseExch      | "ref:00'00'00'00'00'00'00'00" |
| 13: ClockStart        | <empty>                       |
| 14: ClockEnd          | <empty>                       |
| 15: Limit             | <empty>                       |
| 16: Uses              | <empty>                       |
| 17: Log               | <empty>                       |
| 18: LogTo             | <empty>                       |
```

As you can see, all users have an `Enabled` cell, which is initially set to zero. You can enable that user by setting that cell to a one:

```
Locking[ Admin1 ]> set Authority::User1 5 1
```

Let's also set a proper username by setting `CommonName` and change the password. You can set up several users like this, but if it's only you using the drive you might just want to use the same password as for SID and Admin1.

```
Locking[ Admin1 ]> set Authority::User1 2
Reading value until you type 'END' on a new line:
"TheLegend27" # Only really useful for the PBA
END
Locking[ Admin1 ]> passwd User1
New password: ***
Retype password: ***
```

Let's now turn on locking for the global range. This effectively locks all sectors of the device.

```
Locking[ Admin1 ]> set Locking::GlobalRange 5 1 # Turn on read locking
Locking[ Admin1 ]> set Locking::GlobalRange 6 1 # Turn on write locking
Locking[ Admin1 ]> set Locking::GlobalRange 7 1 # Current state to RD locked
Locking[ Admin1 ]> set Locking::GlobalRange 8 1 # Current state to WR locked
```

Optionally, erase all data on the global range by generating a fresh media encryption key:

```
Locking[ Admin1 ]> get Locking::GlobalRange 10 # Column 10 contains the UID
"ref:00'00'08'06'00'00'00'01" # UID of the encryption key
Locking[ Admin1 ]> gen-mek 0x0000080600000001 # Generate new MEK
```

Let's give access rights to the user so that they can unlock the global range:

```
Locking[ Admin1 ]> columns ACE
| Number | Name        | IsUnique | Type                                                                 |
+--------+-------------+----------+----------------------------------------------------------------------+
| 0      | UID         |          | bytes_8                                                              |
| 1      | Name        | yes      | max_bytes_32                                                         |
| 2      | CommonName  | yes      | max_bytes_32                                                         |
| 3      | BooleanExpr |          | list{ typeOr{ 00000C05:objref{ Authority } | 0000040E:uinteger_2 } } |
| 4      | Columns     |          | list{ uinteger_2 }                                                   |
Locking[ Admin1 ]> get ACE::Locking_GlobalRange_Set_RdLocked 3
[{"type":"00'00'0C'05","value":"ref:00'00'00'09'00'00'00'02"}]
Locking[ Admin1 ]> find Authority::User1
UID:  0x0000000900030001
Name: Authority::User1
Locking[ Admin1 ]> set ACE::Locking_GlobalRange_Set_RdLocked 3
Reading value until you type 'END' on a new line:
[
  {
    "type": "00'00'0C'05",
    "value": "ref:00'00'00'09'00'00'00'02"
  },
  {
    "type": "00'00'0C'05",
    "value": "ref:00'00'00'09'00'03'00'01"
  },
  {
    "type": "00'00'04'0E",
    "value": 1
  }
]
END
Locking[ Admin1 ]> set ACE::Locking_GlobalRange_Set_WrLocked 3
Reading value until you type 'END' on a new line:
[
  {
    "type": "00'00'0C'05",
    "value": "ref:00'00'00'09'00'00'00'02"
  },
  {
    "type": "00'00'0C'05",
    "value": "ref:00'00'00'09'00'03'00'01"
  },
  {
    "type": "00'00'04'0E",
    "value": 1
  }
]
END
```

Finally, check if everything works:

```
Locking[ Admin1 ]> end
<no session>> start Locking
Locking[ Anybody ]> auth User1
Password: ***
Locking[ User1 ]> set Locking::GlobalRange 7 0
Locking[ User1 ]> set Locking::GlobalRange 8 0
Locking[ User1 ]> exit # Exits SEDManager
```

The drive should be unlocked until we lock it again or power cycle the drive.

#### Enabling MBR shadowing

After doing the above steps, you should be able to figure this out on your own. In short, you have to change the `Enable` column of the `MBRControl::MBRControl` object using `set`. Furthermore, just like with the locking ranges, you have to give access rights to users to unshadow the MBR on bootup using the `ACE::MBRControl_Set_DoneToDOR` object.


#### Reverting the drive to its manufacturing state (PSID revert)
<a name="psid_revert"></a>

First, find your PSID password. This password is usually printed on the label on the drive.

Once you have the PSID password, launch SEDManager in interactive mode and start a session on the drive as Admin:

```shell
<no session>> start Admin
```

Next, authenticate as PSID using your PSID password

```shell
Admin[ Anybody ]> auth PSID
Password: *** # PSID password
```

Finally, perform revert on the Admin SP:

```shell
Admin[ PSID ]> revert SP::Admin
<no session>>
```

Now your device is back to its original manufacturing state, and ALL DATA IS ERASED. (This also terminates your current session, but you can start a new one as Admin right away and take ownership of the device.)


### PBA mode

Launch SEDManager in PBA mode by typing:

```
sudo SEDManagerCLI --pba
```

Follow the prompts in the shell and login with your username and password:
```
Unlock 'Samsung SSD 980 PRO 2TB':
Login: TheLegend27
Password:
Unlocked (RW) Locking::GlobalRange!
```

SEDManager will inform you if it could unlock any locking ranges with your credentials or if could unshadow the MBR. In this case, the global range was unlocked for both reading and writing. I haven't set up the MBR shadowing, so that was not processed by the PBA.

The main use for PBA mode is putting SEDManager in a bootable PBA environment that you write to the shadow MBR. This way, you can unlock your drive on boot and launch your operating system from your encrypted drive.

## Licensing

I'm using GPLv3 to distribute SEDManager. Be aware of the contributor assignment agreement (CAA).