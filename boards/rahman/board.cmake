
board_runner_args(uf2 "--board-id=RPI-RP2")
board_runner_args(jlink "--device=RP2040_M0_0")
board_runner_args(pyocd "--target=rp2040")

include(${ZEPHYR_BASE}/boards/common/uf2.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)

