# 开启错误退出
$ErrorActionPreference = "Stop"

# 确保 Chocolatey 本身已安装
if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
}

# 更新 Chocolatey 本体
choco upgrade chocolatey -y
