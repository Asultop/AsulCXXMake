# Alpha-v0.2.5 AMake Release

- `AMake --install` : 安装到本地（加入系统 PATH）
- `AMake xxx.cpp` : 自动识别文件类型，提供快速编译与运行
- `AMake run` : 运行上次构建的目标
- `AMake make` : 重建上次构建的文件（例如文件被删除时）
- `AMake remake` : 重新构建上次构建的文件

发布日期: 2025-10-30

修复/改进:

- 修复：当缓存文件包含无效或字符串格式的 JSON 时，程序不再直接把字符串当作 JSON 对象使用并崩溃；现在会尝试解析并在解析失败时回退为空对象，然后重新写入有效缓存。

---
Alpha-v0.2.5
---

## Alpha v0.2.5 — 2025-10-28 (基于 `alpha-v0.1.4` → `HEAD`)

简短说明
- 本次发布集中于增强多文件支持、改进构建与环境管理（支持 `.env`），以及若干格式与运行/重建相关的 bug 修复。

主要新增与改进
- 支持 MultiFile（多文件构建支持） — 提交 fe3b812
	详情：允许在一次构建流程中同时处理多个源文件，改善多文件项目的使用体验。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/fe3b812b220a27607bdf3c6ee1ecfe5e4c6500bc

- 添加 program_suffix 用于区分编译器 — 提交 387bf9b
	详情：引入 program_suffix 配置项，用于在同一系统中区分不同编译器产生的可执行文件，避免命名冲突。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/387bf9b2abaf1b6e9fa062f2213861d96db10fe2

- 支持 `.env` 环境文件 — 提交 291589d
	详情：加入对 `.env` 的读取/支持，可通过环境文件快速设置构建所需的环境变量（例如编译器路径、选项等）。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/291589d63f8962a3de8ca707c9dba265035de97c

- 更新构建流程以初始化可执行文件 — 提交 04afab7
	详情：在构建工作流中增加对可执行文件的初始化步骤，减少首次运行或自动化脚本时找不到可执行文件的问题。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/04afab71100daf1e7ce22ce1d5ae8976cd382a4e

- 优化 JSON 参数获取与环境管理 — 提交 4be9ece
	详情：重构了读取/解析 JSON 参数的逻辑，提高容错（避免将字符串当作 JSON 对象访问导致的崩溃），并改进了环境相关的管理逻辑。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/4be9ece06568f0365a68c382c910edf918f6a9e3

- PrintMap -> F（格式/打印相关调整） — 提交 302226f
	详情：对 `AsulFormatString` 中的打印/格式函数重命名及行为微调，提升格式化输出的一致性。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/302226fcfd37bd46c98d2516c1475eb6a897ca48

修复（Bug fixes）
- 修复运行/重建（run/remake）相关的 bug — 提交 063e7b3
	详情：修复在 run/remake 场景下可能导致失败的边缘情况（详见提交信息）。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/063e7b324a0f59159b9c24774cefb586f8978c52

- 修复 `AsulCXXMake.cpp` 中的格式问题 — 提交 a21566e
	详情：代码格式调整与小错修复，改善可读性并修复潜在格式相关问题。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/a21566e1f7c4cdfab4d35c5dc0842c04bc9359cc

- 修复编译时间计算过快问题 — 提交 b3d68c3
	详情：修正了导致编译时间显示不正确（过快）的逻辑错误，确保时间戳与构建信息正确反映实际耗时。
	Commit: https://github.com/Asultop/AsulCXXMake/commit/b3d68c3aab784093dbdd1eba289e9dc5e464d795

- 修改 versionprint 相关格式 — 提交 b9b5853 / d237010
	详情：调整版本打印（versionprint）的输出格式与样式，使信息更易读且一致。
	Commits:
	- https://github.com/Asultop/AsulCXXMake/commit/b9b585320a3885e76f6ebd57dc5ad6db60e8ee58
	- https://github.com/Asultop/AsulCXXMake/commit/d237010d0f03291d5933d6a340962a8369a2561e

合并/其它
- 多次合并主分支/远程（合并提交） — b233e91 / 8fd55db
	说明：合并远程变更与主分支更新，含若干合并提交，不含直接功能内容。
	Commits:
	- https://github.com/Asultop/AsulCXXMake/commit/b233e914109c360acdfbcc866f802ac93b22a2e8
	- https://github.com/Asultop/AsulCXXMake/commit/8fd55db6520a8e8ea9b68a952e0c23a71dc70441

附注
- 本节为自动聚合草稿，基于提交信息做了简要注释；若需更详细的变更说明（例如在每条中展开影响范围、代码位置或示例），我可以按你的需求继续扩展。
- 若你希望把 PR 链接替代为 commit 链接，或将若干相关 commit 合并为一个更高层次的条目（例如“构建系统改进”），告诉我如何分组即可。

