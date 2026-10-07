"""
Автономный скрипт атомарного коммита и отправки изменений (git commit + push)
в удаленный репозиторий GitHub.

Правила проекта:
- Запускается строго по прямому указанию пользователя.
- Выполняет АТОМАРНО локальный коммит (commit) и удаленный пуш (push).

Использование:
    python git_commit_push.py "Описание изменений"
или без аргументов (будет предложено ввести сообщение или использовано автосообщение).
"""

import sys
import subprocess
import datetime

def run_cmd(cmd, check=True):
    result = subprocess.run(cmd, shell=True, text=True, capture_output=True)
    if result.stdout.strip():
        print(result.stdout.strip())
    if result.stderr.strip() and result.returncode != 0:
        print(f"[ОШИБКА]: {result.stderr.strip()}", file=sys.stderr)
    if check and result.returncode != 0:
        sys.exit(result.returncode)
    return result

def main():
    print("=" * 70)
    print("   АТОМАРНЫЙ GIT COMMIT + PUSH В GITHUB")
    print("=" * 70)

    # 1. Проверка текущей ветки
    branch_res = run_cmd("git rev-parse --abbrev-ref HEAD")
    current_branch = branch_res.stdout.strip()
    print(f"Текущая ветка: {current_branch}")

    # 2. Проверка статуса рабочей копии
    status_res = run_cmd("git status --short", check=False)
    if not status_res.stdout.strip():
        print("Нет изменений для коммита. Рабочая копия чиста.")
        return

    print("\nИзмененные / новые файлы:")
    print(status_res.stdout.strip())
    print("-" * 70)

    # 3. Определение сообщения коммита
    if len(sys.argv) > 1:
        commit_message = " ".join(sys.argv[1:]).strip()
    else:
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M")
        commit_message = f"Update: {timestamp} [Заместитель начальника ОП Смирнов С.Г.]"

    print(f"\nСообщение коммита:\n>>> {commit_message}")

    # 4. Добавление файлов в индекс (git add)
    print("\n[1/3] Выполняется: git add -A ...")
    run_cmd("git add -A")

    # 5. Локальный коммит (git commit)
    print("[2/3] Выполняется: git commit ...")
    # Экранируем двойные кавычки для безопасности в shell
    escaped_msg = commit_message.replace('"', '\\"')
    commit_res = run_cmd(f'git commit -m "{escaped_msg}"', check=False)
    if commit_res.returncode != 0:
        if "nothing to commit" in commit_res.stdout or "nothing to commit" in commit_res.stderr:
            print("Нет изменений для фиксации.")
            return
        else:
            print(f"Ошибка коммита: {commit_res.stderr}", file=sys.stderr)
            sys.exit(commit_res.returncode)

    # 6. Удаленный пуш (git push)
    print(f"[3/3] Выполняется: git push origin {current_branch} ...")
    push_res = run_cmd(f"git push origin {current_branch}")

    print("\n" + "=" * 70)
    print("   АТОМАРНЫЙ КОММИТ И ПУШ В GITHUB УСПЕШНО ЗАВЕРШЕНЫ!")
    print("=" * 70)

if __name__ == "__main__":
    main()
