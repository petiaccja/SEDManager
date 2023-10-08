# SED Manager

## Introduction

### What is SEDManager?

SEDManager is a library and application to manage self-encrypting drives that comply with the Trusted Computing Group specifications, such as Opal and Pyrite.

### Current state of SEDManager

The application is in an early stage of development and as such is not ready for serious use.

The current license is proprietary, meaning you're only allowed to look at the code. I will switch to an appropriate license once it's clear what scheme is the best choice.

### Design goals

- Recognize all Storage Subsystem Classes (see [Resources](#resources) below)
- Be resilient to the differences between the many SSCs and optional features
    - Have a clean and robust code base so that less-tested scenarios are likely to work
    - Report errors in an informative way to the user
    - Be easy to extend and modify to improve support
- Have both a graphical and a command line interface


## TCG Resources
<a name="resources"></a>

| Document | Specification |
|---|---|
| Core v2 | https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage_Architecture_Core_Spec_v2.01_r1.00.pdf |
| PSID | https://trustedcomputinggroup.org/resource/tcg-storage-opal-feature-set-psid/ |


| SSC | Specification |
|---|---|
| Opal v2 | https://trustedcomputinggroup.org/wp-content/uploads/TCG-Storage-Opal-SSC-v2p02-r1p0_pub24jan2022.pdf |
| Opalite v1 | https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opalite_SSC_v1.00_r1.00.pdf |
| Pyrite v2 | https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Pyrite_SSC_v2p01_r1p00_pub.pdf |
| Enterprise v1 | https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-SSC_Enterprise-v1.01_r1.00.pdf |
| Ruby v1 | https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage_SSC_Ruby_v1_r1_pub-1.pdf |
| Key Per I/O v1 | https://trustedcomputinggroup.org/wp-content/uploads/TCG-Storage-Key-Per-IO-SSC-Version1.00-Revision-1.41_pub.pdf |