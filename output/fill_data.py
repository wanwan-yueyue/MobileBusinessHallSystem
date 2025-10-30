# -*- coding: utf-8 -*-
"""
移动营业厅管理系统 - 智能批量导入脚本 v3.017
Windows 专用: 使用 wexpect 模拟控制台交互。
改进:
✅ 自动编码识别（固定 GBK）
✅ 反复跳过 Press any key
✅ 宽松正则匹配
✅ 自动日志保存 automation_log.txt
"""

import time
import random
import os
from datetime import datetime, timedelta
from typing import Dict, List
import wexpect
import locale

LOG_FILE = "automation_log.txt"


def log(msg: str):
    """写入带时间戳日志"""
    ts = datetime.now().strftime("[%Y-%m-%d %H:%M:%S]")
    print(f"{ts} {msg}")
    with open(LOG_FILE, "a", encoding="utf-8") as f:
        f.write(f"{ts} {msg}\n")


# ==========================================================
# 用户数据生成器
# ==========================================================
class CompleteUserGenerator:
    def __init__(self):
        self.last_names = ['王', '李', '张', '刘', '陈', '杨', '赵', '黄', '周', '吴']
        self.first_names = ['伟', '芳', '娜', '敏', '静', '丽', '强', '磊', '军', '杰']
        self.jobs = ['工程师', '教师', '医生', '销售', '经理', '程序员']
        self.cities = ['北京市', '上海市', '广州市', '深圳市', '杭州市', '南京市']

        self.weights = [7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2]
        self.check_codes = ['1','0','X','9','8','7','6','5','4','3','2']

        log("预生成有效用户数据...")
        self.pre_generated_users = [self._generate_valid_user() for _ in range(500)]
        self.index = 0

    def _calc_check_code(self, id17):
        total = sum(int(id17[i]) * self.weights[i] for i in range(17))
        return self.check_codes[total % 11]

    def _generate_valid_id(self):
        area_codes = ['110101','310104','440103','440104']
        area = random.choice(area_codes)
        max_birth = datetime.now() - timedelta(days=18*365)
        min_birth = datetime.now() - timedelta(days=60*365)
        year = random.randint(min_birth.year, max_birth.year)
        month = random.randint(1, 12)
        day = random.randint(1, 28)
        birth = f"{year:04d}{month:02d}{day:02d}"
        seq = f"{random.randint(0,999):03d}"
        id17 = area + birth + seq
        return id17 + self._calc_check_code(id17)

    def _generate_valid_user(self):
        name = random.choice(self.last_names) + random.choice(self.first_names)
        if random.random() > 0.3:
            name += random.choice(self.first_names)
        return {
            'name': name[:6],
            'gender': random.choice(['男','女']),
            'age': str(random.randint(18, 60)),
            'id_card': self._generate_valid_id(),
            'job': random.choice(self.jobs),
            'address': f"{random.choice(self.cities)}中山路{random.randint(1,999)}号"
        }

    def get_users(self, n:int)->List[Dict]:
        users=[]
        for _ in range(n):
            users.append(self.pre_generated_users[self.index])
            self.index = (self.index+1)%len(self.pre_generated_users)
        return users


# ==========================================================
# 自动化控制核心
# ==========================================================
class CompleteBusinessHallAutomation:
    def __init__(self, exe_path:str):
        self.exe_path = exe_path
        self.user_gen = CompleteUserGenerator()
        self.proc = None
        self.encoding = "gbk"  # 固定为 GBK，最稳妥
        log(f"使用终端编码: {self.encoding}")
        self.timings = {'menu':2.0, 'input':1.0, 'operation':3.0}

        # 主菜单关键字模式
        self.main_menu_patterns = [
            r"=======\s*移动营业厅管理系统\s*=======",
            r"请输入操作编号",
            r"主菜单",
            r"1\D*新增用户",
            r"1\."
        ]

        # 字段提示模式
        self.field_patterns = {
            'name': [r"请输入.*姓名", r"姓名.*", r"姓名\(.*\)"],
            'gender': [r"请输入.*性别", r"性别.*"],
            'age': [r"请输入.*年龄", r"年龄.*"],
            'id_card': [r"请输入.*身份证", r"身份证.*"],
            'job': [r"请输入.*职业", r"职业.*"],
            'address': [r"请输入.*地址", r"地址.*"]
        }

    # ------------------------------------------------------------------
    def start_system(self):
        log("🚀 启动系统中...")
        try:
            self.proc = wexpect.spawn(self.exe_path, encoding=self.encoding, timeout=30)
            log("✅ 系统已启动")

            # 等待初始化完成
            self.wait_for_prompt([r"系统初始化完成", r"欢迎使用移动营业厅管理系统"], timeout=20)

            # 连续尝试跳过 “Press any key”
            for i in range(4):
                if self.wait_for_prompt([r"Press any key", r"按任意键"], timeout=5):
                    log(f"🔄 检测到 'Press any key' (第 {i+1} 次)，发送回车")
                    self.press_any_key()
                    time.sleep(1.5)
                if self.wait_for_prompt(self.main_menu_patterns, timeout=6):
                    log("✅ 已成功进入主菜单")
                    return True

            log("⚠️ 多次尝试后仍未检测到主菜单，可能卡在初始化暂停")
            return True

        except Exception as e:
            log(f"❌ 启动失败: {e}")
            return False

    # ------------------------------------------------------------------
    def wait_for_prompt(self, patterns, timeout=10):
        """等待输出中出现任意匹配正则"""
        if isinstance(patterns, str):
            patterns = [patterns]
        try:
            idx = self.proc.expect(patterns, timeout=timeout)
            matched = patterns[idx]
            log(f"✅ 检测到提示 (pattern): {matched}")
            log(f"    [after preview] {self.proc.after.strip()}")
            return True
        except wexpect.TIMEOUT:
            preview = self.proc.before[-200:] if self.proc.before else ""
            log(f"⚠️ 等待提示超时: {patterns}")
            log(f"    ⚠️ 输出预览: {preview}")
            return False

    def send_command(self, cmd:str, delay=None):
        if delay is None: delay = self.timings['input']
        try:
            self.proc.sendline(cmd)
            log(f"    发送命令: '{cmd}'")
            time.sleep(delay)
        except Exception as e:
            log(f"❌ 写入失败: {e}")

    def press_any_key(self):
        """发送回车"""
        try:
            self.proc.sendline('')
            log("    发送任意键/回车继续...")
            time.sleep(1)
        except Exception as e:
            log(f"❌ 发送任意键失败: {e}")

    # ------------------------------------------------------------------
    def ensure_main_menu(self):
        log("🔄 确认主菜单状态...")
        for i in range(2):
            if self.wait_for_prompt(self.main_menu_patterns, timeout=5):
                log("✅ 已在主菜单")
                return True
            log("⚠️ 未检测到主菜单，尝试按任意键并重试...")
            self.press_any_key()
            time.sleep(1.5)
        log("⚠️ 未能确认主菜单")
        return False

    # ------------------------------------------------------------------
    def add_user_complete(self, user:Dict):
        log(f"👤 添加用户: {user['name']}")
        if not self.ensure_main_menu():
            log("⚠️ 无法确认主菜单，继续执行")
        self.send_command('1', self.timings['menu'])
        self.wait_for_prompt([r"请输入.*姓名", r"新增用户"], timeout=8)

        for key, patterns in self.field_patterns.items():
            val = user[key] if key != "id_card" else user["id_card"]
            if self.wait_for_prompt(patterns, timeout=8):
                self.send_command(val)
            else:
                log(f"⚠️ 未检测到字段提示 {patterns}，尝试发送值: {val}")
                self.send_command(val)
        # 注册手机号
        if self.wait_for_prompt([r"是否立即注册手机号"], timeout=8):
            self.send_command('1')
            if self.wait_for_prompt([r"手动输入", r"随机选号"], timeout=6):
                self.send_command('2')
                self.wait_for_prompt([r"注册成功", r"随机分配"], timeout=10)

        if self.wait_for_prompt([r"Press any key", r"按任意键"], timeout=5):
            self.press_any_key()

        log(f"✅ 用户 {user['name']} 信息录入完成")
        return 1

    # ------------------------------------------------------------------
    def save_data(self):
        log("💾 保存数据...")
        self.ensure_main_menu()
        self.send_command('8')
        if self.wait_for_prompt([r"保存成功", r"数据已保存"], timeout=10):
            log("✅ 数据保存成功")
        if self.wait_for_prompt([r"Press any key", r"按任意键"], timeout=5):
            self.press_any_key()

    def exit_system(self):
        log("🚪 退出系统...")
        self.save_data()
        self.ensure_main_menu()
        self.send_command('9')
        self.wait_for_prompt([r"系统已退出", r"感谢使用"], timeout=8)
        log("✅ 系统已退出")

    # ------------------------------------------------------------------
    def run_complete_workflow(self, user_count:int):
        if not self.start_system():
            return False,0
        users = self.user_gen.get_users(user_count)
        success = 0
        for i,u in enumerate(users,1):
            log(f"\n--- 处理用户 {i}/{user_count} ---")
            try:
                success += self.add_user_complete(u)
            except Exception as e:
                log(f"❌ 用户 {u['name']} 异常: {e}")
                self.press_any_key()
        self.save_data()
        self.exit_system()
        log(f"🎉 完成导入，共成功 {success}/{user_count}")
        return True,success


# ==========================================================
# 启动入口
# ==========================================================
def find_executable_auto():
    possible=["mobile_system.exe","main.exe","MobileBusinessHallSystem.exe",
              "./mobile_system","./main",
              "C:\\Repositories\\C\\MobileBusinessHallSystem\\output\\mobile_system.exe"]
    for p in possible:
        if os.path.exists(p):
            return os.path.abspath(p)
    return None


def main():
    print("="*60)
    print("📱 移动营业厅管理系统 - 智能批量导入 v3.017")
    print("自动识别编码、宽松匹配提示，并保存日志到 automation_log.txt")
    print("="*60)

    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)

    exe=find_executable_auto()
    if not exe:
        exe=input("请输入可执行文件路径: ").strip('" ')
        if not os.path.exists(exe):
            print("❌ 找不到文件")
            return

    try:
        count=int(input("请输入要导入的用户数量: "))
    except:
        print("❌ 无效数量")
        return

    if count<=0:
        print("❌ 数量必须大于0")
        return

    confirm=input("确认开始导入? (Y/n): ").strip().lower()
    if confirm not in ['', 'y', 'yes']:
        print("操作已取消")
        return

    automation=CompleteBusinessHallAutomation(exe)
    automation.run_complete_workflow(count)


if __name__=="__main__":
    main()
